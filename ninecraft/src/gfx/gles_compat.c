#include <ninecraft/gfx/gles_compat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* MCPE's GL ES shader source uses ESSL-only syntax (`precision mediump
 * float;` and friends). Apple/NVIDIA/AMDGPU-PRO tend to compile this fine
 * under a desktop GL 2.0 compatibility context, but stricter compilers
 * (notably Mesa's radeonsi/GLSL front end, common on cheap AMD APU
 * laptops) reject the `precision` qualifier outright and fail the
 * compile. If MCPE's own glGetShaderiv/glGetProgramiv error-checking
 * doesn't surface that failure visibly, the symptom is: whatever geometry
 * that shader draws silently never appears, while anything still drawn
 * via the legacy fixed-function path (e.g. a title-screen panorama) does.
 *
 * We defensively comment out `precision ... ;` statements before handing
 * the source to the driver. This is a no-op on drivers that already
 * accepted the qualifier, and unblocks the ones that don't. */
static char *strip_precision_qualifiers(const char *src) {
    size_t len = strlen(src);
    char *out = (char *)malloc(len + 1);
    if (!out) {
        return NULL;
    }
    memcpy(out, src, len + 1);

    for (size_t i = 0; i < len; ) {
        /* look for a `precision` token at a statement boundary */
        if ((i == 0 || out[i - 1] == '\n' || isspace((unsigned char)out[i - 1])) &&
            strncmp(out + i, "precision", 9) == 0 &&
            (i + 9 == len || isspace((unsigned char)out[i + 9]))) {
            size_t j = i;
            while (j < len && out[j] != ';') {
                if (out[j] != '\n') {
                    out[j] = ' ';
                }
                ++j;
            }
            if (j < len) {
                out[j] = ' '; /* blank the trailing ';' too */
            }
            i = j + 1;
        } else {
            ++i;
        }
    }
    return out;
}

static void check_shader_compile(GLuint shader) {
    GLint status = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        char *log = (char *)malloc((size_t)log_len + 1);
        if (log) {
            glGetShaderInfoLog(shader, log_len, NULL, log);
            fprintf(stderr, "[ninecraft] shader %u failed to compile:\n%s\n", shader, log);
            free(log);
        }
    }
}

static void check_program_link(GLuint program) {
    GLint status = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        char *log = (char *)malloc((size_t)log_len + 1);
        if (log) {
            glGetProgramInfoLog(program, log_len, NULL, log);
            fprintf(stderr, "[ninecraft] program %u failed to link:\n%s\n", program, log);
            free(log);
        }
    }
}

FLOAT_ABI_FIX void gl_alpha_func(GLenum func, GLclampf ref) {
    glAlphaFunc(func, ref);
}

void gl_bind_buffer(GLenum target, GLuint buffer) {
    glBindBuffer(target, buffer);
}

void gl_bind_texture(GLenum target, GLuint texture) {
    glBindTexture(target, texture);
}

void gl_blend_func(GLenum sfactor, GLenum dfactor) {
    glBlendFunc(sfactor, dfactor);
}

void gl_buffer_data(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
    glBufferData(target, size, data, usage);
}

void gl_clear(GLbitfield mask) {
    glClear(mask);
}

FLOAT_ABI_FIX void gl_clear_color(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
    glClearColor(red, green, blue, alpha);
}

FLOAT_ABI_FIX void gl_color_4_f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
    glColor4f(red, green, blue, alpha);
}

void gl_color_mask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
    glColorMask(red, green, blue, alpha);
}

void gl_color_pointer(GLint size, GLenum type, GLsizei stride, const void *pointer) {
    glColorPointer(size, type, stride, pointer);
}

void gl_cull_face(GLenum mode) {
    glCullFace(mode);
}

void gl_delete_buffers(GLsizei n, const GLuint *buffers) {
    glDeleteBuffers(n, buffers);
}

void gl_delete_textures(GLsizei n, const GLuint *textures) {
    glDeleteTextures(n, textures);
}

void gl_depth_func(GLenum func) {
    glDepthFunc(func);
}

void gl_depth_mask(GLboolean flag) {
    glDepthMask(flag);
}

FLOAT_ABI_FIX void gl_depth_range_f(GLclampf near, GLclampf far) {
    glDepthRange((GLclampd)near, (GLclampd)far);
}

void gl_disable(GLenum cap) {
    glDisable(cap);
}

void gl_disable_client_state(GLenum array) {
    glDisableClientState(array);
}

void gl_draw_arrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
}

void gl_enable(GLenum cap) {
    glEnable(cap);
}

void gl_enable_client_state(GLenum array) {
    glEnableClientState(array);
}

FLOAT_ABI_FIX void gl_fog_f(GLenum pname, GLfloat param) {
    glFogf(pname, param);
}

FLOAT_ABI_FIX void gl_fog_f_v(GLenum pname, const GLfloat *params) {
    glFogfv(pname, params);
}

void gl_fog_x(GLenum pname, GLfixed param) {
    glFogi(pname, param);
}

void gl_gen_textures(GLsizei n, GLuint *textures) {
    glGenTextures(n, textures);
}

FLOAT_ABI_FIX void gl_get_float_v(GLenum pname, GLfloat *params) {
    glGetFloatv(pname, params);
}

const GLubyte *gl_get_string(GLenum name) {
    return glGetString(name);
}

void gl_hint(GLenum target, GLenum mode) {
    glHint(target, mode);
}

FLOAT_ABI_FIX void gl_line_width(GLfloat width) {
    glLineWidth(width);
}

void gl_load_identity() {
    glLoadIdentity();
}

void gl_matrix_mode(GLenum mode) {
    glMatrixMode(mode);
}

FLOAT_ABI_FIX void gl_mult_matrix_f(const GLfloat *m) {
    glMultMatrixf(m);
}

FLOAT_ABI_FIX void gl_normal_3_f(GLfloat nx, GLfloat ny, GLfloat nz) {
    glNormal3f(nx, ny, nz);
}

FLOAT_ABI_FIX void gl_ortho_f(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far) {
    glOrtho((GLdouble)left, (GLdouble)right, (GLdouble)bottom, (GLdouble)top, (GLdouble)near, (GLdouble)far);
}

FLOAT_ABI_FIX void gl_polygon_offset(GLfloat factor, GLfloat units) {
    glPolygonOffset(factor, units);
}

void gl_pop_matrix() {
    glPopMatrix();
}

void gl_push_matrix() {
    glPushMatrix();
}

void gl_read_pixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) {
    glReadPixels(x, y, width, height, format, type, pixels);
}

FLOAT_ABI_FIX void gl_rotate_f(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    glRotatef(angle, x, y, z);
}

FLOAT_ABI_FIX void gl_scale_f(GLfloat x, GLfloat y, GLfloat z) {
    glScalef(x, y, z);
}

void gl_scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
    glScissor(x, y, width, height);
}

void gl_shade_model(GLenum mode) {
    glShadeModel(mode);
}

void gl_tex_coord_pointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
    glTexCoordPointer(size, type, stride, pointer);
}

void gl_tex_image_2_d(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) {
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void gl_tex_parameter_i(GLenum target, GLenum pname, GLint param) {
    glTexParameteri(target, pname, param);
}

void gl_tex_sub_image_2_d(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

FLOAT_ABI_FIX void gl_translate_f(GLfloat x, GLfloat y, GLfloat z) {
    glTranslatef(x, y, z);
}

void gl_vertex_pointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) {
    glVertexPointer(size, type, stride, pointer);
}

void gl_viewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    glViewport(x, y, width, height);
}

void gl_draw_elements(GLenum mode, GLsizei count, GLenum type, const void *indices) {
    glDrawElements(mode, count, type, indices);
}

GLenum gl_get_error() {
    return glGetError();
}

void gl_gen_buffers(GLsizei n, GLuint *buffers) {
    glGenBuffers(n, buffers);
}

void gl_stencil_func(GLenum func, GLint ref, GLuint mask) {
    glStencilFunc(func, ref, mask);
}

void gl_stencil_mask(GLuint mask) {
    glStencilMask(mask);
}

FLOAT_ABI_FIX void gl_light_model_f(GLenum pname, GLfloat param) {
    glLightModelf(pname, param);
}

FLOAT_ABI_FIX void gl_light_f_v(GLenum light, GLenum pname, const GLfloat *params) {
    glLightfv(light, pname, params);
}

void gl_normal_pointer(GLenum type, GLsizei stride, const GLvoid *pointer) {
    glNormalPointer(type, stride, pointer);
}

void gl_stencil_op(GLenum fail, GLenum zfail, GLenum zpass) {
    glStencilOp(fail, zfail, zpass);
}

void gl_active_texture(GLenum texture) {
    glActiveTexture(texture);
}

void gl_attach_shader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);
}

void gl_clear_stencil(GLint s) {
    glClearStencil(s);
}

void gl_compile_shader(GLuint shader) {
    glCompileShader(shader);
    check_shader_compile(shader);
}

GLuint gl_create_program() {
    return glCreateProgram();
}

GLuint gl_create_shader(GLenum type) {
    return glCreateShader(type);
}

void gl_delete_program(GLuint program) {
    glDeleteProgram(program);
}

void gl_enable_vertex_attrib_array(GLuint index) {
    glEnableVertexAttribArray(index);
}

void gl_get_active_attrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name) {
    glGetActiveAttrib(program, index, bufSize, length, size, type, name);
}

void gl_get_active_uniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name) {
    glGetActiveUniform(program, index, bufSize, length, size, type, name);
}

GLint gl_get_attrib_location(GLuint program, const GLchar *name) {
    return glGetAttribLocation(program, name);
}

void gl_get_program_info_log(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
    glGetProgramInfoLog(program, bufSize, length, infoLog);
}

void gl_get_program_i_v(GLuint program, GLenum pname, GLint *params) {
    glGetProgramiv(program, pname, params);
}

void gl_get_shader_info_log(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
    glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

void gl_get_shader_i_v(GLuint shader, GLenum pname, GLint *params) {
    glGetShaderiv(shader, pname, params);
}

void gl_get_shader_precision_format(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision) {
    glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
}

GLint gl_get_uniform_location(GLuint program, const GLchar *name) {
    return glGetUniformLocation(program, name);
}

void gl_link_program(GLuint program) {
    glLinkProgram(program);
    check_program_link(program);
}

void gl_release_shader_compiler() {
    glReleaseShaderCompiler();
}

void gl_shader_source(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length) {
    char **stripped = (char **)malloc(sizeof(char *) * (size_t)count);
    if (!stripped) {
        glShaderSource(shader, count, string, length);
        return;
    }
    for (GLsizei i = 0; i < count; ++i) {
        stripped[i] = strip_precision_qualifiers(string[i]);
        if (!stripped[i]) {
            /* allocation failed partway through: fall back to the
             * original, unmodified source for this call */
            for (GLsizei k = 0; k < i; ++k) {
                free(stripped[k]);
            }
            free(stripped);
            glShaderSource(shader, count, string, length);
            return;
        }
    }
    glShaderSource(shader, count, (const GLchar *const *)stripped, length);
    for (GLsizei i = 0; i < count; ++i) {
        free(stripped[i]);
    }
    free(stripped);
}

FLOAT_ABI_FIX void gl_uniform_1_f_v(GLint location, GLsizei count, const GLfloat *value) {
    glUniform1fv(location, count, value);
}

void gl_uniform_1_i_v(GLint location, GLsizei count, const GLint *value) {
    glUniform1iv(location, count, value);
}

FLOAT_ABI_FIX void gl_uniform_2_f_v(GLint location, GLsizei count, const GLfloat *value) {
    glUniform2fv(location, count, value);
}

void gl_uniform_2_i_v(GLint location, GLsizei count, const GLint *value) {
    glUniform2iv(location, count, value);
}

FLOAT_ABI_FIX void gl_uniform_3_f_v(GLint location, GLsizei count, const GLfloat *value) {
    glUniform3fv(location, count, value);
}

void gl_uniform_3_i_v(GLint location, GLsizei count, const GLint *value) {
    glUniform3iv(location, count, value);
}

FLOAT_ABI_FIX void gl_uniform_4_f_v(GLint location, GLsizei count, const GLfloat *value) {
    glUniform4fv(location, count, value);
}

void gl_uniform_4_i_v(GLint location, GLsizei count, const GLint *value) {
    glUniform4iv(location, count, value);
}

FLOAT_ABI_FIX void gl_uniform_matrix_2_f_v(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    glUniformMatrix2fv(location, count, transpose, value);
}

FLOAT_ABI_FIX void gl_uniform_matrix_3_f_v(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    glUniformMatrix3fv(location, count, transpose, value);
}

FLOAT_ABI_FIX void gl_uniform_matrix_4_f_v(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    glUniformMatrix4fv(location, count, transpose, value);
}

void gl_use_program(GLuint program) {
    glUseProgram(program);
}

void gl_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void gl_stencil_func_separate(GLenum face, GLenum func, GLint ref, GLuint mask) {
    glStencilFuncSeparate(face, func, ref, mask);
}

void gl_stencil_op_separate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) {
    glStencilOpSeparate(face, sfail, dpfail, dppass);
}

void gl_delete_shader(GLuint shader) {
    glDeleteShader(shader);
}

void gl_uniform_1_i(GLint location, GLint v0) {
    glUniform1i(location, v0);
}

void gl_buffer_sub_data(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) {
    glBufferSubData(target, offset, size, data);
}