/*
 * owl.c
 *
 * Copyright (c) 2022 Xiongfei Shi. All rights reserved.
 *
 * Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 *
 * This file is part of Owl.
 * Usage of Owl is subject to the appropriate license agreement.
 */

#include "SDL_gpu.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "owl.h"
#include "owl_font.h"
#include "owl_framerate.h"
#include "owl_sound.h"

#define OWL_TEXTURED_VERTEX_SHADER_SOURCE                              \
  "#version 400\n\
\
in vec4 gpu_Vertex;\n\
in vec2 gpu_TexCoord;\n\
in vec4 gpu_Color;\n\
uniform mat4 gpu_ModelViewProjectionMatrix;\n\
\
out vec4 color;\n\
out vec2 texCoord;\n\
\
void main(void)\n\
{\n\
	color = gpu_Color;\n\
	texCoord = vec2(gpu_TexCoord);\n\
	gl_Position = gpu_ModelViewProjectionMatrix * gpu_Vertex;\n\
}"

#define OWL_UNTEXTURED_VERTEX_SHADER_SOURCE                            \
  "#version 400\n\
\
in vec4 gpu_Vertex;\n\
in vec4 gpu_Color;\n\
uniform mat4 gpu_ModelViewProjectionMatrix;\n\
\
out vec4 color;\n\
\
void main(void)\n\
{\n\
	color = gpu_Color;\n\
	gl_Position = gpu_ModelViewProjectionMatrix * gpu_Vertex;\n\
}"

#define OWL_WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI

typedef struct owl_Window {
  SDL_Window *window;
  GPU_Target *renderer;
  owl_Canvas *texture;
  GPU_Target *target;
  owl_Pixel color;
  owl_FrameRate fps;
  s32 width, height;
} owl_Window;

static owl_Window owl_app = {0};
static owl_Window *app = &owl_app;

#define OWL_COLOR *(SDL_Color *)&app->color

owl_Pixel owl_rgb(u8 r, u8 g, u8 b) {
  owl_Pixel p = {r, g, b, 0xFF};
  return p;
}

owl_Pixel owl_rgba(u8 r, u8 g, u8 b, u8 a) {
  owl_Pixel p = {r, g, b, a};
  return p;
}

const char *owl_version(s32 *major, s32 *minor, s32 *patch) {
  static char owlver[] = {OWL_RELEASE};

  if (major)
    *major = OWL_MAJOR;

  if (minor)
    *minor = OWL_MINOR;

  if (patch)
    *patch = OWL_PATCH;

  return owlver;
}

u64 owl_ticks(void) {
  u64 ticks = SDL_GetTicks64();
  return ticks > 0 ? ticks : 1;
}

void owl_sleep(u32 ms) { SDL_Delay(ms); }

static void owl_enableGPU3D(GPU_Context *context) {
  if (context->default_textured_shader_program > 0) {
    GPU_DetachShader(context->default_textured_shader_program,
                     context->default_textured_vertex_shader_id);
    GPU_FreeShader(context->default_textured_vertex_shader_id);

    context->default_textured_vertex_shader_id =
        GPU_CompileShader(GPU_VERTEX_SHADER, OWL_TEXTURED_VERTEX_SHADER_SOURCE);
    GPU_AttachShader(context->default_textured_shader_program,
                     context->default_textured_vertex_shader_id);
    GPU_LinkShaderProgram(context->default_textured_shader_program);
  }

  if (context->default_untextured_shader_program > 0) {
    GPU_DetachShader(context->default_untextured_shader_program,
                     context->default_untextured_vertex_shader_id);
    GPU_FreeShader(context->default_untextured_vertex_shader_id);

    context->default_untextured_vertex_shader_id = GPU_CompileShader(
        GPU_VERTEX_SHADER, OWL_UNTEXTURED_VERTEX_SHADER_SOURCE);
    GPU_AttachShader(context->default_untextured_shader_program,
                     context->default_untextured_vertex_shader_id);
    GPU_LinkShaderProgram(context->default_untextured_shader_program);
  }
}

bool owl_init(s32 width, s32 height, const char *title, s32 flags) {
  s32 x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    goto error;

  SDL_DisableScreenSaver();

  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING, "1");

  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  app->window = SDL_CreateWindow(title, x, y, width, height, OWL_WINDOW_FLAGS);

  if (!app->window)
    goto error;

  GPU_SetInitWindow(SDL_GetWindowID(app->window));

  app->renderer = GPU_Init(width, height, GPU_DEFAULT_INIT_FLAGS);

  if (!app->renderer)
    goto error;

  owl_enableGPU3D(app->renderer->context);

  app->texture = GPU_CreateImage(width, height, GPU_FORMAT_RGBA);

  if (!app->texture)
    goto error;

  GPU_SetBlendMode(app->texture, GPU_BLEND_NORMAL);
  GPU_SetBlending(app->texture, true);

  GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
  GPU_SetShapeBlending(true);

  app->target = GPU_GetTarget(app->texture);
  GPU_SetActiveTarget(app->target);

  app->color = owl_rgba(0, 0, 0, 0);
  app->width = width;
  app->height = height;

  if (!owl_fontInit())
    goto error;

  if (!owl_soundInit())
    goto error;

  owl_setFPS(OWL_FRAMERATE_DEFAULT);
  return true;

error:
  owl_quit();
  return false;
}

void owl_quit(void) {
  owl_soundQuit();
  owl_fontQuit();

  if (app->texture) {
    GPU_FreeImage(app->texture);
    app->texture = NULL;
  }

  if (app->renderer) {
    GPU_FreeTarget(app->renderer);
    app->renderer = NULL;
  }

  if (app->window) {
    SDL_DestroyWindow(app->window);
    app->window = NULL;
  }

  GPU_Quit();
  SDL_Quit();
}

bool owl_setFPS(u32 rate) { return owl_frameRateSet(&app->fps, rate); }

u32 owl_getFPS(void) { return app->fps.rate; }

u32 owl_wait(void) { return owl_frameRateWait(&app->fps); }

owl_Canvas *owl_screen(void) { return app->texture; }

owl_Canvas *owl_canvas(s32 width, s32 height) {
  owl_Canvas *canvas = GPU_CreateImage(width, height, GPU_FORMAT_RGBA);

  if (!canvas)
    return NULL;

  GPU_SetBlendMode(canvas, GPU_BLEND_NORMAL);
  GPU_SetBlending(canvas, true);

  return canvas;
}

static SDL_Surface *owl_surface(const u8 *data, s32 w, s32 h, u8 format) {
  s32 d = format * 8;
  s32 p = format * w;
  s32 f = (format == OWL_FORMAT_RGB) ? SDL_PIXELFORMAT_RGB24
                                     : SDL_PIXELFORMAT_RGBA32;
  if (!data)
    return NULL;

  return SDL_CreateRGBSurfaceWithFormatFrom((void *)data, w, h, d, p, f);
}

static owl_Canvas *owl_fromSurface(SDL_Surface *surface, bool free_surface) {
  owl_Canvas *canvas = GPU_CopyImageFromSurface(surface);

  if (free_surface)
    SDL_FreeSurface(surface);

  if (!canvas)
    return NULL;

  GPU_SetBlendMode(canvas, GPU_BLEND_NORMAL);
  GPU_SetBlending(canvas, true);

  return canvas;
}

owl_Canvas *owl_image(const u8 *data, s32 w, s32 h, u8 format) {
  SDL_Surface *surface = owl_surface(data, w, h, format);

  if (!surface)
    return NULL;

  return owl_fromSurface(surface, true);
}

owl_Canvas *owl_imagex(const u8 *data, s32 w, s32 h, owl_Pixel colorkey) {
  SDL_Surface *surface = owl_surface(data, w, h, OWL_FORMAT_RGB);
  u32 key;

  if (!surface)
    return NULL;

  key = SDL_MapRGB(surface->format, colorkey.r, colorkey.g, colorkey.b);
  SDL_SetColorKey(surface, SDL_TRUE, key);

  return owl_fromSurface(surface, true);
}

owl_Canvas *owl_load(const char *filename) {
  owl_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = owl_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

owl_Canvas *owl_loadex(const char *filename, owl_Pixel colorkey) {
  owl_Canvas *canvas;
  s32 w, h, format;
  u8 *data;

  if (!filename)
    return NULL;

  data = stbi_load(filename, &w, &h, &format, 0);

  if (!data)
    return NULL;

  canvas = (format == STBI_rgb) ? owl_imagex(data, w, h, colorkey)
                                : owl_image(data, w, h, format);
  stbi_image_free(data);

  return canvas;
}

void owl_freeCanvas(owl_Canvas *canvas) {
  if (canvas == app->texture)
    return;

  if (GPU_GetTarget(canvas) == app->target)
    owl_target(app->texture);

  if (canvas)
    GPU_FreeImage(canvas);
}

void owl_size(owl_Canvas *canvas, s32 *w, s32 *h) {
  if (w)
    *w = canvas->w;

  if (h)
    *h = canvas->h;
}

static void owl_shapeBlendMode(owl_Canvas *canvas) {
  GPU_BlendMode b = canvas->blend_mode;

  GPU_SetShapeBlendFunction(b.source_color, b.dest_color, b.source_alpha,
                            b.dest_alpha);
  GPU_SetShapeBlendEquation(b.color_equation, b.alpha_equation);

  GPU_SetShapeBlending(GPU_GetBlending(canvas));
}

void owl_blendMode(owl_Canvas *canvas, s32 mode) {
  switch (mode) {
  case OWL_BLEND_ALPHA:
    GPU_SetBlendMode(canvas, GPU_BLEND_NORMAL);
    GPU_SetBlending(canvas, true);
    break;
  case OWL_BLEND_NONE:
  default:
    GPU_SetBlending(canvas, false);
    break;
  }

  if (GPU_GetTarget(canvas) == app->target)
    owl_shapeBlendMode(canvas);
}

void owl_target(owl_Canvas *canvas) {
  GPU_Target *target;

  if (!canvas)
    canvas = app->texture;

  target = GPU_GetTarget(canvas);

  if (target != app->target) {
    app->target = target;

    GPU_SetActiveTarget(app->target);
    owl_shapeBlendMode(canvas);
  }
}

void owl_thickness(f32 thickness) { GPU_SetLineThickness(thickness); }

void owl_color(owl_Pixel color) { app->color = color; }

void owl_clear(void) {
  owl_Pixel color = app->color;
  GPU_ClearRGBA(app->target, color.r, color.g, color.b, color.a);
}

void owl_pixel(f32 x, f32 y) {
  GPU_Pixel(app->target, x, y, OWL_COLOR);
}

void owl_line(f32 x1, f32 y1, f32 x2, f32 y2) {
  GPU_Line(app->target, x1, y1, x2, y2, OWL_COLOR);
}

void owl_rect(f32 x, f32 y, f32 w, f32 h) {
  GPU_Rect rect = {x, y, w, h};
  GPU_Rectangle2(app->target, rect, OWL_COLOR);
}

void owl_fillRect(f32 x, f32 y, f32 w, f32 h) {
  GPU_Rect rect = {x, y, w, h};
  GPU_RectangleFilled2(app->target, rect, OWL_COLOR);
}

void owl_arc(f32 x, f32 y, f32 radius, f32 start_angle, f32 end_angle) {
  GPU_Arc(app->target, x, y, radius, start_angle, end_angle, OWL_COLOR);
}

void owl_fillArc(f32 x, f32 y, f32 radius, f32 start_angle, f32 end_angle) {
  GPU_ArcFilled(app->target, x, y, radius, start_angle, end_angle, OWL_COLOR);
}

void owl_circle(f32 x, f32 y, f32 radius) {
  GPU_Circle(app->target, x, y, radius, OWL_COLOR);
}

void owl_fillCircle(f32 x, f32 y, f32 radius) {
  GPU_CircleFilled(app->target, x, y, radius, OWL_COLOR);
}

void owl_ellipse(f32 x, f32 y, f32 rx, f32 ry, f32 degrees) {
  GPU_Ellipse(app->target, x, y, rx, ry, degrees, OWL_COLOR);
}

void owl_fillEllipse(f32 x, f32 y, f32 rx, f32 ry, f32 degrees) {
  GPU_EllipseFilled(app->target, x, y, rx, ry, degrees, OWL_COLOR);
}

void owl_sector(f32 x, f32 y, f32 inner_radius, f32 outer_radius,
                f32 start_angle, f32 end_angle) {
  GPU_Sector(app->target, x, y, inner_radius, outer_radius, start_angle,
             end_angle, OWL_COLOR);
}

void owl_fillSector(f32 x, f32 y, f32 inner_radius, f32 outer_radius,
                      f32 start_angle, f32 end_angle) {
  GPU_SectorFilled(app->target, x, y, inner_radius, outer_radius, start_angle,
                   end_angle, OWL_COLOR);
}

void owl_tri(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3) {
  GPU_Tri(app->target, x1, y1, x2, y2, x3, y3, OWL_COLOR);
}

void owl_fillTri(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3) {
  GPU_TriFilled(app->target, x1, y1, x2, y2, x3, y3, OWL_COLOR);
}

void owl_rectRound(f32 x, f32 y, f32 w, f32 h, f32 radius) {
  GPU_Rect rect = {x, y, w, h};
  GPU_RectangleRound2(app->target, rect, radius, OWL_COLOR);
}

void owl_fillRectRound(f32 x, f32 y, f32 w, f32 h, f32 radius) {
  GPU_Rect rect = {x, y, w, h};
  GPU_RectangleRoundFilled2(app->target, rect, radius, OWL_COLOR);
}

void owl_polygon(const owl_Point *points, s32 num_points, bool close) {
  GPU_Polyline(app->target, num_points, (f32 *)points, OWL_COLOR, close);
}

void owl_fillPolygon(const owl_Point *points, s32 num_points) {
  GPU_PolygonFilled(app->target, num_points, (f32 *)points, OWL_COLOR);
}

void owl_geometry(owl_Canvas *texture, s32 type, const owl_Vertex *vertices,
                  s32 num_vertices, const u16 *indices, s32 num_indices) {
  GPU_PrimitiveBatchV(texture, app->target, type, num_vertices,
                      (void *)vertices, num_indices, (u16 *)indices,
                      GPU_BATCH_XY_ST_RGBA8);
}

void owl_clip(const owl_Rect *rect) {
  if (rect)
    GPU_SetClipRect(app->target, *(GPU_Rect *)rect);
  else
    GPU_UnsetClip(app->target);
}

void owl_viewport(const owl_Rect *rect) {
  if (rect)
    GPU_SetViewport(app->target, *(GPU_Rect *)rect);
  else
    GPU_UnsetViewport(app->target);
}

void owl_blit(owl_Canvas *canvas, const owl_Rect *srcrect,
              const owl_Rect *dstrect, f32 degrees, const owl_Point *center,
              u8 flip) {
  f32 pivot_x, pivot_y;

  if (center) {
    pivot_x = center->x;
    pivot_y = center->y;
  } else {
    pivot_x = (srcrect ? srcrect->w : canvas->w) * 0.5f;
    pivot_y = (srcrect ? srcrect->h : canvas->h) * 0.5f;
  }

  GPU_BlitRectX(canvas, (GPU_Rect *)srcrect, app->target, (GPU_Rect *)dstrect,
                degrees, pivot_x, pivot_y, flip);
}

void owl_present(void) {
  GPU_BlitRect(app->texture, NULL, app->renderer, NULL);
  GPU_Flip(app->renderer);
}

void owl_matrixMode3D(int matrix_mode) {
  GPU_MatrixMode(app->target, matrix_mode);
}

void owl_matrixMultiply3D(const owl_Matrix4 *matrix) {
  GPU_MultMatrix(matrix->m);
}

void owl_pushMatrix3D(void) { GPU_PushMatrix(); }

void owl_popMatrix3D(void) { GPU_PopMatrix(); }

void owl_loadMatrix3D(const owl_Matrix4 *matrix) { GPU_LoadMatrix(matrix->m); }

void owl_loadIdentity3D(void) { GPU_LoadIdentity(); }

void owl_ortho3D(f32 left, f32 right, f32 bottom, f32 top, f32 z_near,
                      f32 z_far) {
  GPU_Ortho(left, right, bottom, top, z_near, z_far);
}

void owl_frustum3D(f32 left, f32 right, f32 bottom, f32 top, f32 z_near,
                        f32 z_far) {
  GPU_Frustum(left, right, bottom, top, z_near, z_far);
}

void owl_perspective3D(f32 fovy, f32 aspect, f32 z_near, f32 z_far) {
  GPU_Perspective(fovy, aspect, z_near, z_far);
}

void owl_lookAt3D(f32 eye_x, f32 eye_y, f32 eye_z, f32 target_x,
                       f32 target_y, f32 target_z, f32 up_x, f32 up_y,
                       f32 up_z) {
  GPU_LookAt(eye_x, eye_y, eye_z, target_x, target_y, target_z, up_x, up_y,
             up_z);
}

void owl_translate3D(f32 x, f32 y, f32 z) { GPU_Translate(x, y, z); }

void owl_scale3D(f32 sx, f32 sy, f32 sz) { GPU_Scale(sx, sy, sz); }

void owl_rotate3D(f32 degrees, f32 x, f32 y, f32 z) {
  GPU_Rotate(degrees, x, y, z);
}

void owl_resetProjection3D(void) { GPU_ResetProjection(app->target); }

void owl_modelViewProjection3D(owl_Matrix4 *result) {
  GPU_GetModelViewProjection(result->m);
}

void owl_setMatrix3D(int matrix_mode, const owl_Matrix4 *matrix) {
  if (matrix_mode == GPU_MODEL)
    GPU_SetModel(matrix->m);
  else if (matrix_mode == GPU_VIEW)
    GPU_SetView(matrix->m);
  else // if(matrix_mode == GPU_PROJECTION)
    GPU_SetProjection(matrix->m);
}

owl_Matrix4 *owl_getMatrix3D(int matrix_mode) {
  if (matrix_mode == GPU_MODEL)
    return (owl_Matrix4 *)GPU_GetModel();
  else if (matrix_mode == GPU_VIEW)
    return (owl_Matrix4 *)GPU_GetView();
  else // if(matrix_mode == GPU_PROJECTION)
    return (owl_Matrix4 *)GPU_GetProjection();
}

owl_Matrix4 *owl_currentMatrix3D(void) {
  return (owl_Matrix4 *)GPU_GetCurrentMatrix();
}

void owl_begin3D(void) {
  GPU_FlushBlitBuffer();

  owl_matrixMode3D(OWL_MODEL);
  owl_pushMatrix3D();
  owl_loadIdentity3D();
  owl_matrixMode3D(OWL_VIEW);
  owl_pushMatrix3D();
  owl_loadIdentity3D();
  owl_matrixMode3D(OWL_PROJECTION);
  owl_pushMatrix3D();
  owl_loadIdentity3D();
}

void owl_end3D(void) {
  GPU_ResetRendererState();

  owl_matrixMode3D(OWL_MODEL);
  owl_popMatrix3D();
  owl_matrixMode3D(OWL_VIEW);
  owl_popMatrix3D();
  owl_matrixMode3D(OWL_PROJECTION);
  owl_popMatrix3D();
}

void owl_zbuffer(owl_Canvas *canvas) {
  GPU_AddDepthBuffer(GPU_GetTarget(canvas));
}

void owl_depthTest(bool enable) {
  GPU_SetDepthTest(app->target, enable);
}

void owl_depthFunction(s32 op) { GPU_SetDepthFunction(app->target, op); }

void owl_geometry3D(owl_Canvas *texture, s32 type, const owl_Vertex3D *vertices,
                    s32 num_vertices, const u16 *indices, s32 num_indices) {
  GPU_PrimitiveBatchV(texture, app->target, type, num_vertices,
                      (void *)vertices, num_indices, (u16 *)indices,
                      GPU_BATCH_XYZ_ST_RGBA8);
}
