///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Extensions.cpp
//
// Following file contains functions to grab extensions function pointers
// 
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Created by: Vilem Otte <vilem.otte@post.cz>
//
// Log:
// - Initial file created
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Extensions.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.3 Core
#ifdef _WIN32
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage = NULL;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.4 Core
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements = NULL;
PFNGLPOINTPARAMETERFPROC glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv = NULL;
PFNGLPOINTPARAMETERIPROC glPointParameteri = NULL;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 1.5 Core
PFNGLGENQUERIESPROC glGenQueries = NULL;
PFNGLDELETEQUERIESPROC glDeleteQueries = NULL;
PFNGLISQUERYPROC glIsQuery = NULL;
PFNGLBEGINQUERYPROC glBeginQuery = NULL;
PFNGLENDQUERYPROC glEndQuery = NULL;
PFNGLGETQUERYIVPROC glGetQueryiv = NULL;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLISBUFFERPROC glIsBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = NULL;
PFNGLMAPBUFFERPROC glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 2.0 Core
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = NULL;
PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC glGetShaderSource = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv = NULL;
PFNGLISPROGRAMPROC glIsProgram = NULL;
PFNGLISSHADERPROC glIsShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM2FPROC glUniform2f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM2IPROC glUniform2i = NULL;
PFNGLUNIFORM3IPROC glUniform3i = NULL;
PFNGLUNIFORM4IPROC glUniform4i = NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLUNIFORM1IVPROC glUniform1iv = NULL;
PFNGLUNIFORM2IVPROC glUniform2iv = NULL;
PFNGLUNIFORM3IVPROC glUniform3iv = NULL;
PFNGLUNIFORM4IVPROC glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 2.1 Core
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 3.0 Core

// GL_ARB_framebuffer_object 
PFNGLISRENDERBUFFERPROC glIsRenderbuffer = NULL;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv = NULL;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer = NULL;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = NULL;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer = NULL;

// GL_ARB_map_buffer_range 
PFNGLMAPBUFFERRANGEPROC glMapBufferRange = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange = NULL;

// GL_ARB_vertex_array_object 
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLISVERTEXARRAYPROC glIsVertexArray = NULL;

PFNGLCOLORMASKIPROC glColorMaski = NULL;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v = NULL;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v = NULL;
PFNGLENABLEIPROC glEnablei = NULL;
PFNGLDISABLEIPROC glDisablei = NULL;
PFNGLISENABLEDIPROC glIsEnabledi = NULL;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback = NULL;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback = NULL;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange = NULL;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying = NULL;
PFNGLCLAMPCOLORPROC glClampColor = NULL;
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender = NULL;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = NULL;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv = NULL;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv = NULL;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i = NULL;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i = NULL;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i = NULL;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i = NULL;
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui = NULL;
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui = NULL;
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui = NULL;
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui = NULL;
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv = NULL;
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv = NULL;
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv = NULL;
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv = NULL;
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv = NULL;
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv = NULL;
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv = NULL;
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv = NULL;
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv = NULL;
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv = NULL;
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv = NULL;
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv = NULL;
PFNGLGETUNIFORMUIVPROC glGetUniformuiv = NULL;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation = NULL;
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation = NULL;
PFNGLUNIFORM1UIPROC glUniform1ui = NULL;
PFNGLUNIFORM2UIPROC glUniform2ui = NULL;
PFNGLUNIFORM3UIPROC glUniform3ui = NULL;
PFNGLUNIFORM4UIPROC glUniform4ui = NULL;
PFNGLUNIFORM1UIVPROC glUniform1uiv = NULL;
PFNGLUNIFORM2UIVPROC glUniform2uiv = NULL;
PFNGLUNIFORM3UIVPROC glUniform3uiv = NULL;
PFNGLUNIFORM4UIVPROC glUniform4uiv = NULL;
PFNGLTEXPARAMETERIIVPROC glTexParameterIiv = NULL;
PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv = NULL;
PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv = NULL;
PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv = NULL;
PFNGLCLEARBUFFERIVPROC glClearBufferiv = NULL;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv = NULL;
PFNGLCLEARBUFFERFVPROC glClearBufferfv = NULL;
PFNGLCLEARBUFFERFIPROC glClearBufferfi = NULL;
PFNGLGETSTRINGIPROC glGetStringi = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 3.1 Core

// GL_ARB_copy_buffer 
PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData = NULL;

// GL_ARB_uniform_buffer_object 
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices = NULL;
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv = NULL;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName = NULL;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = NULL;

PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced = NULL;
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced = NULL;
PFNGLTEXBUFFERPROC glTexBuffer = NULL;
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 3.2 Core

// GL_ARB_draw_elements_base_vertex 
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex = NULL;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex = NULL;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex = NULL;

// GL_ARB_provoking_vertex 
PFNGLPROVOKINGVERTEXPROC glProvokingVertex = NULL;

// GL_ARB_sync 
PFNGLFENCESYNCPROC glFenceSync = NULL;
PFNGLISSYNCPROC glIsSync = NULL;
PFNGLDELETESYNCPROC glDeleteSync = NULL;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync = NULL;
PFNGLWAITSYNCPROC glWaitSync = NULL;
PFNGLGETINTEGER64VPROC glGetInteger64v = NULL;
PFNGLGETSYNCIVPROC glGetSynciv = NULL;

// GL_ARB_texture_multisample 
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = NULL;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample = NULL;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv = NULL;
PFNGLSAMPLEMASKIPROC glSampleMaski = NULL;

PFNGLGETINTEGER64I_VPROC glGetInteger64i_v = NULL;
PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v = NULL;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 3.3 Core

// GL_ARB_blend_func_extended 
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed = NULL;
PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex = NULL;

// GL_ARB_sampler_objects 
PFNGLGENSAMPLERSPROC glGenSamplers = NULL;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = NULL;
PFNGLISSAMPLERPROC glIsSampler = NULL;
PFNGLBINDSAMPLERPROC glBindSampler = NULL;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv = NULL;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf = NULL;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = NULL;
PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv = NULL;
PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv = NULL;
PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv = NULL;
PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv = NULL;
PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv = NULL;
PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv = NULL;

// GL_ARB_explicit_attrib_location, but it has none 

// GL_ARB_occlusion_query2 (no entry points) 

// GL_ARB_shader_bit_encoding (no entry points) 

// GL_ARB_texture_rgb10_a2ui (no entry points) 

// GL_ARB_texture_swizzle (no entry points) 

// GL_ARB_timer_query 
PFNGLQUERYCOUNTERPROC glQueryCounter = NULL;
PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v = NULL;
PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v = NULL;

// GL_ARB_vertex_type_2_10_10_10_rev 
PFNGLVERTEXP2UIPROC glVertexP2ui = NULL;
PFNGLVERTEXP2UIVPROC glVertexP2uiv = NULL;
PFNGLVERTEXP3UIPROC glVertexP3ui = NULL;
PFNGLVERTEXP3UIVPROC glVertexP3uiv = NULL;
PFNGLVERTEXP4UIPROC glVertexP4ui = NULL;
PFNGLVERTEXP4UIVPROC glVertexP4uiv = NULL;
PFNGLTEXCOORDP1UIPROC glTexCoordP1ui = NULL;
PFNGLTEXCOORDP1UIVPROC glTexCoordP1uiv = NULL;
PFNGLTEXCOORDP2UIPROC glTexCoordP2ui = NULL;
PFNGLTEXCOORDP2UIVPROC glTexCoordP2uiv = NULL;
PFNGLTEXCOORDP3UIPROC glTexCoordP3ui = NULL;
PFNGLTEXCOORDP3UIVPROC glTexCoordP3uiv = NULL;
PFNGLTEXCOORDP4UIPROC glTexCoordP4ui = NULL;
PFNGLTEXCOORDP4UIVPROC glTexCoordP4uiv = NULL;
PFNGLMULTITEXCOORDP1UIPROC glMultiTexCoordP1ui = NULL;
PFNGLMULTITEXCOORDP1UIVPROC glMultiTexCoordP1uiv = NULL;
PFNGLMULTITEXCOORDP2UIPROC glMultiTexCoordP2ui = NULL;
PFNGLMULTITEXCOORDP2UIVPROC glMultiTexCoordP2uiv = NULL;
PFNGLMULTITEXCOORDP3UIPROC glMultiTexCoordP3ui = NULL;
PFNGLMULTITEXCOORDP3UIVPROC glMultiTexCoordP3uiv = NULL;
PFNGLMULTITEXCOORDP4UIPROC glMultiTexCoordP4ui = NULL;
PFNGLMULTITEXCOORDP4UIVPROC glMultiTexCoordP4uiv = NULL;
PFNGLNORMALP3UIPROC glNormalP3ui = NULL;
PFNGLNORMALP3UIVPROC glNormalP3uiv = NULL;
PFNGLCOLORP3UIPROC glColorP3ui = NULL;
PFNGLCOLORP3UIVPROC glColorP3uiv = NULL;
PFNGLCOLORP4UIPROC glColorP4ui = NULL;
PFNGLCOLORP4UIVPROC glColorP4uiv = NULL;
PFNGLSECONDARYCOLORP3UIPROC glSecondaryColorP3ui = NULL;
PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv = NULL;
PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui = NULL;
PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv = NULL;
PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui = NULL;
PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv = NULL;
PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui = NULL;
PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv = NULL;
PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui = NULL;
PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv = NULL;

PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 4.0 Core

// GL_ARB_texture_query_lod (no entry points) 

// GL_ARB_draw_indirect 
PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect = NULL;
PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect = NULL;

// GL_ARB_gpu_shader5 (no entry points) 

// GL_ARB_gpu_shader_fp64 
PFNGLUNIFORM1DPROC glUniform1d = NULL;
PFNGLUNIFORM2DPROC glUniform2d = NULL;
PFNGLUNIFORM3DPROC glUniform3d = NULL;
PFNGLUNIFORM4DPROC glUniform4d = NULL;
PFNGLUNIFORM1DVPROC glUniform1dv = NULL;
PFNGLUNIFORM2DVPROC glUniform2dv = NULL;
PFNGLUNIFORM3DVPROC glUniform3dv = NULL;
PFNGLUNIFORM4DVPROC glUniform4dv = NULL;
PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv = NULL;
PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv = NULL;
PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv = NULL;
PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv = NULL;
PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv = NULL;
PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv = NULL;
PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv = NULL;
PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv = NULL;
PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv = NULL;
PFNGLGETUNIFORMDVPROC glGetUniformdv = NULL;

// GL_ARB_shader_subroutine 
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation = NULL;
PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex = NULL;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv = NULL;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName = NULL;
PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName = NULL;
PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv = NULL;
PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv = NULL;
PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv = NULL;

// GL_ARB_tessellation_shader 
PFNGLPATCHPARAMETERIPROC glPatchParameteri = NULL;
PFNGLPATCHPARAMETERFVPROC glPatchParameterfv = NULL;

// GL_ARB_texture_buffer_object_rgb32 (no entry points) 

// GL_ARB_texture_cube_map_array (no entry points) 

// GL_ARB_texture_gather (no entry points) 

// GL_ARB_transform_feedback2 
PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback = NULL;
PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks = NULL;
PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks = NULL;
PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback = NULL;
PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback = NULL;
PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback = NULL;
PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback = NULL;

// GL_ARB_transform_feedback3 
PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream = NULL;
PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed = NULL;
PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed = NULL;
PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv = NULL;

PFNGLMINSAMPLESHADINGPROC glMinSampleShading = NULL;
PFNGLBLENDEQUATIONIPROC glBlendEquationi = NULL;
PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei = NULL;
PFNGLBLENDFUNCIPROC glBlendFunci = NULL;
PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 4.1 Core

// GL_ARB_ES2_compatibility 
PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler = NULL;
PFNGLSHADERBINARYPROC glShaderBinary = NULL;
PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat = NULL;
PFNGLDEPTHRANGEFPROC glDepthRangef = NULL;
PFNGLCLEARDEPTHFPROC glClearDepthf = NULL;

// GL_ARB_get_program_binary 
PFNGLGETPROGRAMBINARYPROC glGetProgramBinary = NULL;
PFNGLPROGRAMBINARYPROC glProgramBinary = NULL;
PFNGLPROGRAMPARAMETERIPROC glProgramParameteri = NULL;

// GL_ARB_separate_shader_objects 
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages = NULL;
PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram = NULL;
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv = NULL;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline = NULL;
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines = NULL;
PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines = NULL;
PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline = NULL;
PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv = NULL;
PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i = NULL;
PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv = NULL;
PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f = NULL;
PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv = NULL;
PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d = NULL;
PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv = NULL;
PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui = NULL;
PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv = NULL;
PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i = NULL;
PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv = NULL;
PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f = NULL;
PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv = NULL;
PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d = NULL;
PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv = NULL;
PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui = NULL;
PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv = NULL;
PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i = NULL;
PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv = NULL;
PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f = NULL;
PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv = NULL;
PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d = NULL;
PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv = NULL;
PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui = NULL;
PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv = NULL;
PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i = NULL;
PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv = NULL;
PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f = NULL;
PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv = NULL;
PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d = NULL;
PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv = NULL;
PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui = NULL;
PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv = NULL;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv = NULL;
PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline = NULL;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog = NULL;

// GL_ARB_shader_precision (no entry points) 

// GL_ARB_vertex_attrib_64bit 
PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d = NULL;
PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d = NULL;
PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d = NULL;
PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d = NULL;
PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv = NULL;
PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv = NULL;
PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv = NULL;
PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv = NULL;
PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer = NULL;
PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv = NULL;

// GL_ARB_viewport_array 
PFNGLVIEWPORTARRAYVPROC glViewportArrayv = NULL;
PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf = NULL;
PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv = NULL;
PFNGLSCISSORARRAYVPROC glScissorArrayv = NULL;
PFNGLSCISSORINDEXEDPROC glScissorIndexed = NULL;
PFNGLSCISSORINDEXEDVPROC glScissorIndexedv = NULL;
PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv = NULL;
PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed = NULL;
PFNGLGETFLOATI_VPROC glGetFloati_v = NULL;
PFNGLGETDOUBLEI_VPROC glGetDoublei_v = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL 4.2 Core

// GL_ARB_base_instance 
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance = NULL;

// GL_ARB_shading_language_420pack (no entry points) 

// GL_ARB_transform_feedback_instanced 
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced = NULL;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced = NULL;

// GL_ARB_compressed_texture_pixel_storage (no entry points) 

// GL_ARB_conservative_depth (no entry points) 

// GL_ARB_internalformat_query 
PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ = NULL;

// GL_ARB_map_buffer_alignment (no entry points) 

// GL_ARB_shader_atomic_counters 
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv = NULL;

// GL_ARB_shader_image_load_store 
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture = NULL;
PFNGLMEMORYBARRIERPROC glMemoryBarrier = NULL;

// GL_ARB_shading_language_packing (no entry points) 

// GL_ARB_texture_storage 
PFNGLTEXSTORAGE1DPROC glTexStorage1D = NULL;
PFNGLTEXSTORAGE2DPROC glTexStorage2D = NULL;
PFNGLTEXSTORAGE3DPROC glTexStorage3D = NULL;
PFNGLTEXTURESTORAGE1DEXTPROC glTextureStorage1DEXT = NULL;
PFNGLTEXTURESTORAGE2DEXTPROC glTextureStorage2DEXT = NULL;
PFNGLTEXTURESTORAGE3DEXTPROC glTextureStorage3DEXT = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions

unsigned int g_mExtensionsCount;// Count of OpenGL extensions
const char** g_mExtensionsList;	// OpenGL Extensions list

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

#ifndef min
#define min(a,b) (a < b ? a : b)
#endif

/// <summary>
/// IsExtensionSupported, Checks whether mExtension extension is in GPU extensions list
/// <param name="mExtension">Extension name that we want to check</param>
/// </summary>
bool IsExtensionSupported(const char* mExtension)
{
	for (unsigned int i = 0; i < g_mExtensionsCount; i++)
	{
		if (strcmp(g_mExtensionsList[i], mExtension) == 0)
		{
			return true;
		}
	}

	return false;
}

/// <summary>Gets function pointer to each extension
/// <param name="major">Major OpenGL version</param>
/// <param name="minor">Minor OpenGL version</param>
/// </summary>
void CheckExtensions(int* major, int* minor)
{
	int g_mMajor = 4;
	int g_mMinor = 3;

	// Get glGetStringi proc address
	glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress((const char*)"glGetStringi");
	if (!glGetStringi)
	{
		printf("Error: glGetStringi function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	// TODO: If glGetStringi == NULL, use old glGetString(GL_EXTENSIONS);

	glGetIntegerv(GL_NUM_EXTENSIONS, (GLint*)&g_mExtensionsCount);
	g_mExtensionsList = (const char**)malloc(sizeof(char*)* g_mExtensionsCount);
	for (unsigned int i = 0; i < g_mExtensionsCount; i++)
	{
		g_mExtensionsList[i] = (const char*)glGetStringi(GL_EXTENSIONS, i);
	}

	///////////////////////
	// OpenGL 1.3 Core 
#ifdef _WIN32
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress((const char*)"glActiveTexture");
	if (!glActiveTexture)
	{
		printf("Error: glActiveTexture function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)wglGetProcAddress((const char*)"glSampleCoverage");
	if (!glSampleCoverage)
	{
		printf("Error: glSampleCoverage function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)wglGetProcAddress((const char*)"glCompressedTexImage3D");
	if (!glCompressedTexImage3D)
	{
		printf("Error: glCompressedTexImage3D function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress((const char*)"glCompressedTexImage2D");
	if (!glCompressedTexImage2D)
	{
		printf("Error: glCompressedTexImage2D function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)wglGetProcAddress((const char*)"glCompressedTexImage1D");
	if (!glCompressedTexImage1D)
	{
		printf("Error: glCompressedTexImage1D function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)wglGetProcAddress((const char*)"glCompressedTexSubImage3D");
	if (!glCompressedTexSubImage3D)
	{
		printf("Error: glCompressedTexSubImage3D function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)wglGetProcAddress((const char*)"glCompressedTexSubImage2D");
	if (!glCompressedTexSubImage2D)
	{
		printf("Error: glCompressedTexSubImage2D function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)wglGetProcAddress((const char*)"glCompressedTexSubImage1D");
	if (!glCompressedTexSubImage1D)
	{
		printf("Error: glCompressedTexSubImage1D function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}

	glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)wglGetProcAddress((const char*)"glGetCompressedTexImage");
	if (!glGetCompressedTexImage)
	{
		printf("Error: glGetCompressedTexImage function contained in OpenGL 1.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 3);
	}
#endif
	///////////////////////

	///////////////////////
	// OpenGL 1.4 Core 
	glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)wglGetProcAddress((const char*)"glBlendFuncSeparate");
	if (!glBlendFuncSeparate)
	{
		printf("Error: glBlendFuncSeparate function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)wglGetProcAddress((const char*)"glMultiDrawArrays");
	if (!glMultiDrawArrays)
	{
		printf("Error: glMultiDrawArrays function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)wglGetProcAddress((const char*)"glMultiDrawElements");
	if (!glMultiDrawElements)
	{
		printf("Error: glMultiDrawElements function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	glPointParameterf = (PFNGLPOINTPARAMETERFPROC)wglGetProcAddress((const char*)"glPointParameterf");
	if (!glPointParameterf)
	{
		printf("Error: glPointParameterf function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress((const char*)"glPointParameterfv");
	if (!glPointParameterfv)
	{
		printf("Error: glPointParameterfv function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	glPointParameteri = (PFNGLPOINTPARAMETERIPROC)wglGetProcAddress((const char*)"glPointParameteri");
	if (!glPointParameteri)
	{
		printf("Error: glPointParameteri function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)wglGetProcAddress((const char*)"glPointParameteriv");
	if (!glPointParameteriv)
	{
		printf("Error: glPointParameteriv function contained in OpenGL 1.4 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 4);
	}

	///////////////////////

	///////////////////////
	// OpenGL 1.5 Core 
	glGenQueries = (PFNGLGENQUERIESPROC)wglGetProcAddress((const char*)"glGenQueries");
	if (!glGenQueries)
	{
		printf("Error: glGenQueries function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glDeleteQueries = (PFNGLDELETEQUERIESPROC)wglGetProcAddress((const char*)"glDeleteQueries");
	if (!glDeleteQueries)
	{
		printf("Error: glDeleteQueries function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glIsQuery = (PFNGLISQUERYPROC)wglGetProcAddress((const char*)"glIsQuery");
	if (!glIsQuery)
	{
		printf("Error: glIsQuery function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glBeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress((const char*)"glBeginQuery");
	if (!glBeginQuery)
	{
		printf("Error: glBeginQuery function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glEndQuery = (PFNGLENDQUERYPROC)wglGetProcAddress((const char*)"glEndQuery");
	if (!glEndQuery)
	{
		printf("Error: glEndQuery function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGetQueryiv = (PFNGLGETQUERYIVPROC)wglGetProcAddress((const char*)"glGetQueryiv");
	if (!glGetQueryiv)
	{
		printf("Error: glGetQueryiv function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)wglGetProcAddress((const char*)"glGetQueryObjectiv");
	if (!glGetQueryObjectiv)
	{
		printf("Error: glGetQueryObjectiv function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress((const char*)"glGetQueryObjectuiv");
	if (!glGetQueryObjectuiv)
	{
		printf("Error: glGetQueryObjectuiv function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress((const char*)"glBindBuffer");
	if (!glBindBuffer)
	{
		printf("Error: glBindBuffer function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress((const char*)"glDeleteBuffers");
	if (!glDeleteBuffers)
	{
		printf("Error: glDeleteBuffers function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress((const char*)"glGenBuffers");
	if (!glGenBuffers)
	{
		printf("Error: glGenBuffers function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glIsBuffer = (PFNGLISBUFFERPROC)wglGetProcAddress((const char*)"glIsBuffer");
	if (!glIsBuffer)
	{
		printf("Error: glIsBuffer function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress((const char*)"glBufferData");
	if (!glBufferData)
	{
		printf("Error: glBufferData function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress((const char*)"glBufferSubData");
	if (!glBufferSubData)
	{
		printf("Error: glBufferSubData function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)wglGetProcAddress((const char*)"glGetBufferSubData");
	if (!glGetBufferSubData)
	{
		printf("Error: glGetBufferSubData function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress((const char*)"glMapBuffer");
	if (!glMapBuffer)
	{
		printf("Error: glMapBuffer function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress((const char*)"glUnmapBuffer");
	if (!glUnmapBuffer)
	{
		printf("Error: glUnmapBuffer function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)wglGetProcAddress((const char*)"glGetBufferParameteriv");
	if (!glGetBufferParameteriv)
	{
		printf("Error: glGetBufferParameteriv function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)wglGetProcAddress((const char*)"glGetBufferPointerv");
	if (!glGetBufferPointerv)
	{
		printf("Error: glGetBufferPointerv function contained in OpenGL 1.5 core is not supported!\n");
		g_mMajor = min(g_mMajor, 1);
		g_mMinor = min(g_mMinor, 5);
	}

	///////////////////////

	///////////////////////
	// OpenGL 2.0 Core 
	glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)wglGetProcAddress((const char*)"glBlendEquationSeparate");
	if (!glBlendEquationSeparate)
	{
		printf("Error: glBlendEquationSeparate function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress((const char*)"glDrawBuffers");
	if (!glDrawBuffers)
	{
		printf("Error: glDrawBuffers function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)wglGetProcAddress((const char*)"glStencilOpSeparate");
	if (!glStencilOpSeparate)
	{
		printf("Error: glStencilOpSeparate function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)wglGetProcAddress((const char*)"glStencilFuncSeparate");
	if (!glStencilFuncSeparate)
	{
		printf("Error: glStencilFuncSeparate function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)wglGetProcAddress((const char*)"glStencilMaskSeparate");
	if (!glStencilMaskSeparate)
	{
		printf("Error: glStencilMaskSeparate function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress((const char*)"glAttachShader");
	if (!glAttachShader)
	{
		printf("Error: glAttachShader function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress((const char*)"glBindAttribLocation");
	if (!glBindAttribLocation)
	{
		printf("Error: glBindAttribLocation function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress((const char*)"glCompileShader");
	if (!glCompileShader)
	{
		printf("Error: glCompileShader function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress((const char*)"glCreateProgram");
	if (!glCreateProgram)
	{
		printf("Error: glCreateProgram function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress((const char*)"glCreateShader");
	if (!glCreateShader)
	{
		printf("Error: glCreateShader function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress((const char*)"glDeleteProgram");
	if (!glDeleteProgram)
	{
		printf("Error: glDeleteProgram function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress((const char*)"glDeleteShader");
	if (!glDeleteShader)
	{
		printf("Error: glDeleteShader function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress((const char*)"glDetachShader");
	if (!glDetachShader)
	{
		printf("Error: glDetachShader function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress((const char*)"glDisableVertexAttribArray");
	if (!glDisableVertexAttribArray)
	{
		printf("Error: glDisableVertexAttribArray function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress((const char*)"glEnableVertexAttribArray");
	if (!glEnableVertexAttribArray)
	{
		printf("Error: glEnableVertexAttribArray function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress((const char*)"glGetActiveAttrib");
	if (!glGetActiveAttrib)
	{
		printf("Error: glGetActiveAttrib function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress((const char*)"glGetActiveUniform");
	if (!glGetActiveUniform)
	{
		printf("Error: glGetActiveUniform function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress((const char*)"glGetAttachedShaders");
	if (!glGetAttachedShaders)
	{
		printf("Error: glGetAttachedShaders function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress((const char*)"glGetAttribLocation");
	if (!glGetAttribLocation)
	{
		printf("Error: glGetAttribLocation function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress((const char*)"glGetProgramiv");
	if (!glGetProgramiv)
	{
		printf("Error: glGetProgramiv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress((const char*)"glGetProgramInfoLog");
	if (!glGetProgramInfoLog)
	{
		printf("Error: glGetProgramInfoLog function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress((const char*)"glGetShaderiv");
	if (!glGetShaderiv)
	{
		printf("Error: glGetShaderiv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress((const char*)"glGetShaderInfoLog");
	if (!glGetShaderInfoLog)
	{
		printf("Error: glGetShaderInfoLog function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)wglGetProcAddress((const char*)"glGetShaderSource");
	if (!glGetShaderSource)
	{
		printf("Error: glGetShaderSource function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress((const char*)"glGetUniformLocation");
	if (!glGetUniformLocation)
	{
		printf("Error: glGetUniformLocation function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetUniformfv = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress((const char*)"glGetUniformfv");
	if (!glGetUniformfv)
	{
		printf("Error: glGetUniformfv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetUniformiv = (PFNGLGETUNIFORMIVPROC)wglGetProcAddress((const char*)"glGetUniformiv");
	if (!glGetUniformiv)
	{
		printf("Error: glGetUniformiv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)wglGetProcAddress((const char*)"glGetVertexAttribdv");
	if (!glGetVertexAttribdv)
	{
		printf("Error: glGetVertexAttribdv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)wglGetProcAddress((const char*)"glGetVertexAttribfv");
	if (!glGetVertexAttribfv)
	{
		printf("Error: glGetVertexAttribfv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)wglGetProcAddress((const char*)"glGetVertexAttribiv");
	if (!glGetVertexAttribiv)
	{
		printf("Error: glGetVertexAttribiv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)wglGetProcAddress((const char*)"glGetVertexAttribPointerv");
	if (!glGetVertexAttribPointerv)
	{
		printf("Error: glGetVertexAttribPointerv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glIsProgram = (PFNGLISPROGRAMPROC)wglGetProcAddress((const char*)"glIsProgram");
	if (!glIsProgram)
	{
		printf("Error: glIsProgram function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glIsShader = (PFNGLISSHADERPROC)wglGetProcAddress((const char*)"glIsShader");
	if (!glIsShader)
	{
		printf("Error: glIsShader function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress((const char*)"glLinkProgram");
	if (!glLinkProgram)
	{
		printf("Error: glLinkProgram function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress((const char*)"glShaderSource");
	if (!glShaderSource)
	{
		printf("Error: glShaderSource function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress((const char*)"glUseProgram");
	if (!glUseProgram)
	{
		printf("Error: glUseProgram function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress((const char*)"glUniform1f");
	if (!glUniform1f)
	{
		printf("Error: glUniform1f function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress((const char*)"glUniform2f");
	if (!glUniform2f)
	{
		printf("Error: glUniform2f function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress((const char*)"glUniform3f");
	if (!glUniform3f)
	{
		printf("Error: glUniform3f function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress((const char*)"glUniform4f");
	if (!glUniform4f)
	{
		printf("Error: glUniform4f function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress((const char*)"glUniform1i");
	if (!glUniform1i)
	{
		printf("Error: glUniform1i function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform2i = (PFNGLUNIFORM2IPROC)wglGetProcAddress((const char*)"glUniform2i");
	if (!glUniform2i)
	{
		printf("Error: glUniform2i function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform3i = (PFNGLUNIFORM3IPROC)wglGetProcAddress((const char*)"glUniform3i");
	if (!glUniform3i)
	{
		printf("Error: glUniform3i function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform4i = (PFNGLUNIFORM4IPROC)wglGetProcAddress((const char*)"glUniform4i");
	if (!glUniform4i)
	{
		printf("Error: glUniform4i function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress((const char*)"glUniform1fv");
	if (!glUniform1fv)
	{
		printf("Error: glUniform1fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress((const char*)"glUniform2fv");
	if (!glUniform2fv)
	{
		printf("Error: glUniform2fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress((const char*)"glUniform3fv");
	if (!glUniform3fv)
	{
		printf("Error: glUniform3fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress((const char*)"glUniform4fv");
	if (!glUniform4fv)
	{
		printf("Error: glUniform4fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress((const char*)"glUniform1iv");
	if (!glUniform1iv)
	{
		printf("Error: glUniform1iv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress((const char*)"glUniform2iv");
	if (!glUniform2iv)
	{
		printf("Error: glUniform2iv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress((const char*)"glUniform3iv");
	if (!glUniform3iv)
	{
		printf("Error: glUniform3iv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress((const char*)"glUniform4iv");
	if (!glUniform4iv)
	{
		printf("Error: glUniform4iv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress((const char*)"glUniformMatrix2fv");
	if (!glUniformMatrix2fv)
	{
		printf("Error: glUniformMatrix2fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress((const char*)"glUniformMatrix3fv");
	if (!glUniformMatrix3fv)
	{
		printf("Error: glUniformMatrix3fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress((const char*)"glUniformMatrix4fv");
	if (!glUniformMatrix4fv)
	{
		printf("Error: glUniformMatrix4fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress((const char*)"glValidateProgram");
	if (!glValidateProgram)
	{
		printf("Error: glValidateProgram function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)wglGetProcAddress((const char*)"glVertexAttrib1d");
	if (!glVertexAttrib1d)
	{
		printf("Error: glVertexAttrib1d function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)wglGetProcAddress((const char*)"glVertexAttrib1dv");
	if (!glVertexAttrib1dv)
	{
		printf("Error: glVertexAttrib1dv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress((const char*)"glVertexAttrib1f");
	if (!glVertexAttrib1f)
	{
		printf("Error: glVertexAttrib1f function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress((const char*)"glVertexAttrib1fv");
	if (!glVertexAttrib1fv)
	{
		printf("Error: glVertexAttrib1fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)wglGetProcAddress((const char*)"glVertexAttrib1s");
	if (!glVertexAttrib1s)
	{
		printf("Error: glVertexAttrib1s function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)wglGetProcAddress((const char*)"glVertexAttrib1sv");
	if (!glVertexAttrib1sv)
	{
		printf("Error: glVertexAttrib1sv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)wglGetProcAddress((const char*)"glVertexAttrib2d");
	if (!glVertexAttrib2d)
	{
		printf("Error: glVertexAttrib2d function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)wglGetProcAddress((const char*)"glVertexAttrib2dv");
	if (!glVertexAttrib2dv)
	{
		printf("Error: glVertexAttrib2dv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)wglGetProcAddress((const char*)"glVertexAttrib2f");
	if (!glVertexAttrib2f)
	{
		printf("Error: glVertexAttrib2f function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress((const char*)"glVertexAttrib2fv");
	if (!glVertexAttrib2fv)
	{
		printf("Error: glVertexAttrib2fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)wglGetProcAddress((const char*)"glVertexAttrib2s");
	if (!glVertexAttrib2s)
	{
		printf("Error: glVertexAttrib2s function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)wglGetProcAddress((const char*)"glVertexAttrib2sv");
	if (!glVertexAttrib2sv)
	{
		printf("Error: glVertexAttrib2sv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)wglGetProcAddress((const char*)"glVertexAttrib3d");
	if (!glVertexAttrib3d)
	{
		printf("Error: glVertexAttrib3d function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)wglGetProcAddress((const char*)"glVertexAttrib3dv");
	if (!glVertexAttrib3dv)
	{
		printf("Error: glVertexAttrib3dv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress((const char*)"glVertexAttrib3fv");
	if (!glVertexAttrib3fv)
	{
		printf("Error: glVertexAttrib3fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)wglGetProcAddress((const char*)"glVertexAttrib3s");
	if (!glVertexAttrib3s)
	{
		printf("Error: glVertexAttrib3s function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)wglGetProcAddress((const char*)"glVertexAttrib3sv");
	if (!glVertexAttrib3sv)
	{
		printf("Error: glVertexAttrib3sv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)wglGetProcAddress((const char*)"glVertexAttrib4Nbv");
	if (!glVertexAttrib4Nbv)
	{
		printf("Error: glVertexAttrib4Nbv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)wglGetProcAddress((const char*)"glVertexAttrib4Niv");
	if (!glVertexAttrib4Niv)
	{
		printf("Error: glVertexAttrib4Niv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)wglGetProcAddress((const char*)"glVertexAttrib4Nsv");
	if (!glVertexAttrib4Nsv)
	{
		printf("Error: glVertexAttrib4Nsv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)wglGetProcAddress((const char*)"glVertexAttrib4Nub");
	if (!glVertexAttrib4Nub)
	{
		printf("Error: glVertexAttrib4Nub function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)wglGetProcAddress((const char*)"glVertexAttrib4Nubv");
	if (!glVertexAttrib4Nubv)
	{
		printf("Error: glVertexAttrib4Nubv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)wglGetProcAddress((const char*)"glVertexAttrib4Nuiv");
	if (!glVertexAttrib4Nuiv)
	{
		printf("Error: glVertexAttrib4Nuiv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)wglGetProcAddress((const char*)"glVertexAttrib4Nusv");
	if (!glVertexAttrib4Nusv)
	{
		printf("Error: glVertexAttrib4Nusv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)wglGetProcAddress((const char*)"glVertexAttrib4bv");
	if (!glVertexAttrib4bv)
	{
		printf("Error: glVertexAttrib4bv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)wglGetProcAddress((const char*)"glVertexAttrib4d");
	if (!glVertexAttrib4d)
	{
		printf("Error: glVertexAttrib4d function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)wglGetProcAddress((const char*)"glVertexAttrib4dv");
	if (!glVertexAttrib4dv)
	{
		printf("Error: glVertexAttrib4dv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress((const char*)"glVertexAttrib4fv");
	if (!glVertexAttrib4fv)
	{
		printf("Error: glVertexAttrib4fv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)wglGetProcAddress((const char*)"glVertexAttrib4iv");
	if (!glVertexAttrib4iv)
	{
		printf("Error: glVertexAttrib4iv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)wglGetProcAddress((const char*)"glVertexAttrib4s");
	if (!glVertexAttrib4s)
	{
		printf("Error: glVertexAttrib4s function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)wglGetProcAddress((const char*)"glVertexAttrib4sv");
	if (!glVertexAttrib4sv)
	{
		printf("Error: glVertexAttrib4sv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)wglGetProcAddress((const char*)"glVertexAttrib4ubv");
	if (!glVertexAttrib4ubv)
	{
		printf("Error: glVertexAttrib4ubv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)wglGetProcAddress((const char*)"glVertexAttrib4uiv");
	if (!glVertexAttrib4uiv)
	{
		printf("Error: glVertexAttrib4uiv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)wglGetProcAddress((const char*)"glVertexAttrib4usv");
	if (!glVertexAttrib4usv)
	{
		printf("Error: glVertexAttrib4usv function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress((const char*)"glVertexAttribPointer");
	if (!glVertexAttribPointer)
	{
		printf("Error: glVertexAttribPointer function contained in OpenGL 2.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 0);
	}

	///////////////////////

	///////////////////////
	// OpenGL 2.1 Core 
	glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)wglGetProcAddress((const char*)"glUniformMatrix2x3fv");
	if (!glUniformMatrix2x3fv)
	{
		printf("Error: glUniformMatrix2x3fv function contained in OpenGL 2.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 1);
	}

	glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)wglGetProcAddress((const char*)"glUniformMatrix3x2fv");
	if (!glUniformMatrix3x2fv)
	{
		printf("Error: glUniformMatrix3x2fv function contained in OpenGL 2.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 1);
	}

	glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)wglGetProcAddress((const char*)"glUniformMatrix2x4fv");
	if (!glUniformMatrix2x4fv)
	{
		printf("Error: glUniformMatrix2x4fv function contained in OpenGL 2.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 1);
	}

	glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)wglGetProcAddress((const char*)"glUniformMatrix4x2fv");
	if (!glUniformMatrix4x2fv)
	{
		printf("Error: glUniformMatrix4x2fv function contained in OpenGL 2.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 1);
	}

	glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)wglGetProcAddress((const char*)"glUniformMatrix3x4fv");
	if (!glUniformMatrix3x4fv)
	{
		printf("Error: glUniformMatrix3x4fv function contained in OpenGL 2.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 2);
		g_mMinor = min(g_mMinor, 1);
	}

	///////////////////////

	///////////////////////
	// OpenGL 3.0 Core 

	// GL_ARB_framebuffer_object 
	if (IsExtensionSupported("GL_ARB_framebuffer_object") == false)
	{
		printf("Error: Extension GL_ARB_framebuffer_object is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress((const char*)"glIsRenderbuffer");
		if (!glIsRenderbuffer)
		{
			printf("Error: glIsRenderbuffer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress((const char*)"glBindRenderbuffer");
		if (!glBindRenderbuffer)
		{
			printf("Error: glBindRenderbuffer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress((const char*)"glDeleteRenderbuffers");
		if (!glDeleteRenderbuffers)
		{
			printf("Error: glDeleteRenderbuffers function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress((const char*)"glGenRenderbuffers");
		if (!glGenRenderbuffers)
		{
			printf("Error: glGenRenderbuffers function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress((const char*)"glRenderbufferStorage");
		if (!glRenderbufferStorage)
		{
			printf("Error: glRenderbufferStorage function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress((const char*)"glGetRenderbufferParameteriv");
		if (!glGetRenderbufferParameteriv)
		{
			printf("Error: glGetRenderbufferParameteriv function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress((const char*)"glIsFramebuffer");
		if (!glIsFramebuffer)
		{
			printf("Error: glIsFramebuffer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress((const char*)"glBindFramebuffer");
		if (!glBindFramebuffer)
		{
			printf("Error: glBindFramebuffer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress((const char*)"glDeleteFramebuffers");
		if (!glDeleteFramebuffers)
		{
			printf("Error: glDeleteFramebuffers function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress((const char*)"glGenFramebuffers");
		if (!glGenFramebuffers)
		{
			printf("Error: glGenFramebuffers function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress((const char*)"glCheckFramebufferStatus");
		if (!glCheckFramebufferStatus)
		{
			printf("Error: glCheckFramebufferStatus function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress((const char*)"glFramebufferTexture1D");
		if (!glFramebufferTexture1D)
		{
			printf("Error: glFramebufferTexture1D function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress((const char*)"glFramebufferTexture2D");
		if (!glFramebufferTexture2D)
		{
			printf("Error: glFramebufferTexture2D function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress((const char*)"glFramebufferTexture3D");
		if (!glFramebufferTexture3D)
		{
			printf("Error: glFramebufferTexture3D function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress((const char*)"glFramebufferRenderbuffer");
		if (!glFramebufferRenderbuffer)
		{
			printf("Error: glFramebufferRenderbuffer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress((const char*)"glGetFramebufferAttachmentParameteriv");
		if (!glGetFramebufferAttachmentParameteriv)
		{
			printf("Error: glGetFramebufferAttachmentParameteriv function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress((const char*)"glGenerateMipmap");
		if (!glGenerateMipmap)
		{
			printf("Error: glGenerateMipmap function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress((const char*)"glBlitFramebuffer");
		if (!glBlitFramebuffer)
		{
			printf("Error: glBlitFramebuffer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)wglGetProcAddress((const char*)"glRenderbufferStorageMultisample");
		if (!glRenderbufferStorageMultisample)
		{
			printf("Error: glRenderbufferStorageMultisample function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)wglGetProcAddress((const char*)"glFramebufferTextureLayer");
		if (!glFramebufferTextureLayer)
		{
			printf("Error: glFramebufferTextureLayer function in GL_ARB_framebuffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_map_buffer_range 
	if (IsExtensionSupported("GL_ARB_map_buffer_range") == false)
	{
		printf("Error: Extension GL_ARB_map_buffer_range is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)wglGetProcAddress((const char*)"glMapBufferRange");
		if (!glMapBufferRange)
		{
			printf("Error: glMapBufferRange function in GL_ARB_map_buffer_range is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)wglGetProcAddress((const char*)"glFlushMappedBufferRange");
		if (!glFlushMappedBufferRange)
		{
			printf("Error: glFlushMappedBufferRange function in GL_ARB_map_buffer_range is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_vertex_array_object 
	if (IsExtensionSupported("GL_ARB_vertex_array_object") == false)
	{
		printf("Error: Extension GL_ARB_vertex_array_object is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress((const char*)"glBindVertexArray");
		if (!glBindVertexArray)
		{
			printf("Error: glBindVertexArray function in GL_ARB_vertex_array_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress((const char*)"glDeleteVertexArrays");
		if (!glDeleteVertexArrays)
		{
			printf("Error: glDeleteVertexArrays function in GL_ARB_vertex_array_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress((const char*)"glGenVertexArrays");
		if (!glGenVertexArrays)
		{
			printf("Error: glGenVertexArrays function in GL_ARB_vertex_array_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

		glIsVertexArray = (PFNGLISVERTEXARRAYPROC)wglGetProcAddress((const char*)"glIsVertexArray");
		if (!glIsVertexArray)
		{
			printf("Error: glIsVertexArray function in GL_ARB_vertex_array_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	glColorMaski = (PFNGLCOLORMASKIPROC)wglGetProcAddress((const char*)"glColorMaski");
	if (!glColorMaski)
	{
		printf("Error: glColorMaski function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)wglGetProcAddress((const char*)"glGetBooleani_v");
	if (!glGetBooleani_v)
	{
		printf("Error: glGetBooleani_v function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)wglGetProcAddress((const char*)"glGetIntegeri_v");
	if (!glGetIntegeri_v)
	{
		printf("Error: glGetIntegeri_v function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glEnablei = (PFNGLENABLEIPROC)wglGetProcAddress((const char*)"glEnablei");
	if (!glEnablei)
	{
		printf("Error: glEnablei function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glDisablei = (PFNGLDISABLEIPROC)wglGetProcAddress((const char*)"glDisablei");
	if (!glDisablei)
	{
		printf("Error: glDisablei function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glIsEnabledi = (PFNGLISENABLEDIPROC)wglGetProcAddress((const char*)"glIsEnabledi");
	if (!glIsEnabledi)
	{
		printf("Error: glIsEnabledi function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glBeginTransformFeedback");
	if (!glBeginTransformFeedback)
	{
		printf("Error: glBeginTransformFeedback function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glEndTransformFeedback");
	if (!glEndTransformFeedback)
	{
		printf("Error: glEndTransformFeedback function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)wglGetProcAddress((const char*)"glBindBufferRange");
	if (!glBindBufferRange)
	{
		printf("Error: glBindBufferRange function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress((const char*)"glBindBufferBase");
	if (!glBindBufferBase)
	{
		printf("Error: glBindBufferBase function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)wglGetProcAddress((const char*)"glTransformFeedbackVaryings");
	if (!glTransformFeedbackVaryings)
	{
		printf("Error: glTransformFeedbackVaryings function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)wglGetProcAddress((const char*)"glGetTransformFeedbackVarying");
	if (!glGetTransformFeedbackVarying)
	{
		printf("Error: glGetTransformFeedbackVarying function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glClampColor = (PFNGLCLAMPCOLORPROC)wglGetProcAddress((const char*)"glClampColor");
	if (!glClampColor)
	{
		printf("Error: glClampColor function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)wglGetProcAddress((const char*)"glBeginConditionalRender");
	if (!glBeginConditionalRender)
	{
		printf("Error: glBeginConditionalRender function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)wglGetProcAddress((const char*)"glEndConditionalRender");
	if (!glEndConditionalRender)
	{
		printf("Error: glEndConditionalRender function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress((const char*)"glVertexAttribIPointer");
	if (!glVertexAttribIPointer)
	{
		printf("Error: glVertexAttribIPointer function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)wglGetProcAddress((const char*)"glGetVertexAttribIiv");
	if (!glGetVertexAttribIiv)
	{
		printf("Error: glGetVertexAttribIiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)wglGetProcAddress((const char*)"glGetVertexAttribIuiv");
	if (!glGetVertexAttribIuiv)
	{
		printf("Error: glGetVertexAttribIuiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)wglGetProcAddress((const char*)"glVertexAttribI1i");
	if (!glVertexAttribI1i)
	{
		printf("Error: glVertexAttribI1i function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)wglGetProcAddress((const char*)"glVertexAttribI2i");
	if (!glVertexAttribI2i)
	{
		printf("Error: glVertexAttribI2i function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)wglGetProcAddress((const char*)"glVertexAttribI3i");
	if (!glVertexAttribI3i)
	{
		printf("Error: glVertexAttribI3i function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)wglGetProcAddress((const char*)"glVertexAttribI4i");
	if (!glVertexAttribI4i)
	{
		printf("Error: glVertexAttribI4i function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)wglGetProcAddress((const char*)"glVertexAttribI1ui");
	if (!glVertexAttribI1ui)
	{
		printf("Error: glVertexAttribI1ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)wglGetProcAddress((const char*)"glVertexAttribI2ui");
	if (!glVertexAttribI2ui)
	{
		printf("Error: glVertexAttribI2ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)wglGetProcAddress((const char*)"glVertexAttribI3ui");
	if (!glVertexAttribI3ui)
	{
		printf("Error: glVertexAttribI3ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)wglGetProcAddress((const char*)"glVertexAttribI4ui");
	if (!glVertexAttribI4ui)
	{
		printf("Error: glVertexAttribI4ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)wglGetProcAddress((const char*)"glVertexAttribI1iv");
	if (!glVertexAttribI1iv)
	{
		printf("Error: glVertexAttribI1iv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)wglGetProcAddress((const char*)"glVertexAttribI2iv");
	if (!glVertexAttribI2iv)
	{
		printf("Error: glVertexAttribI2iv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)wglGetProcAddress((const char*)"glVertexAttribI3iv");
	if (!glVertexAttribI3iv)
	{
		printf("Error: glVertexAttribI3iv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)wglGetProcAddress((const char*)"glVertexAttribI4iv");
	if (!glVertexAttribI4iv)
	{
		printf("Error: glVertexAttribI4iv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)wglGetProcAddress((const char*)"glVertexAttribI1uiv");
	if (!glVertexAttribI1uiv)
	{
		printf("Error: glVertexAttribI1uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)wglGetProcAddress((const char*)"glVertexAttribI2uiv");
	if (!glVertexAttribI2uiv)
	{
		printf("Error: glVertexAttribI2uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)wglGetProcAddress((const char*)"glVertexAttribI3uiv");
	if (!glVertexAttribI3uiv)
	{
		printf("Error: glVertexAttribI3uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)wglGetProcAddress((const char*)"glVertexAttribI4uiv");
	if (!glVertexAttribI4uiv)
	{
		printf("Error: glVertexAttribI4uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)wglGetProcAddress((const char*)"glVertexAttribI4bv");
	if (!glVertexAttribI4bv)
	{
		printf("Error: glVertexAttribI4bv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)wglGetProcAddress((const char*)"glVertexAttribI4sv");
	if (!glVertexAttribI4sv)
	{
		printf("Error: glVertexAttribI4sv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)wglGetProcAddress((const char*)"glVertexAttribI4ubv");
	if (!glVertexAttribI4ubv)
	{
		printf("Error: glVertexAttribI4ubv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)wglGetProcAddress((const char*)"glVertexAttribI4usv");
	if (!glVertexAttribI4usv)
	{
		printf("Error: glVertexAttribI4usv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)wglGetProcAddress((const char*)"glGetUniformuiv");
	if (!glGetUniformuiv)
	{
		printf("Error: glGetUniformuiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)wglGetProcAddress((const char*)"glBindFragDataLocation");
	if (!glBindFragDataLocation)
	{
		printf("Error: glBindFragDataLocation function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)wglGetProcAddress((const char*)"glGetFragDataLocation");
	if (!glGetFragDataLocation)
	{
		printf("Error: glGetFragDataLocation function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform1ui = (PFNGLUNIFORM1UIPROC)wglGetProcAddress((const char*)"glUniform1ui");
	if (!glUniform1ui)
	{
		printf("Error: glUniform1ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform2ui = (PFNGLUNIFORM2UIPROC)wglGetProcAddress((const char*)"glUniform2ui");
	if (!glUniform2ui)
	{
		printf("Error: glUniform2ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform3ui = (PFNGLUNIFORM3UIPROC)wglGetProcAddress((const char*)"glUniform3ui");
	if (!glUniform3ui)
	{
		printf("Error: glUniform3ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform4ui = (PFNGLUNIFORM4UIPROC)wglGetProcAddress((const char*)"glUniform4ui");
	if (!glUniform4ui)
	{
		printf("Error: glUniform4ui function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform1uiv = (PFNGLUNIFORM1UIVPROC)wglGetProcAddress((const char*)"glUniform1uiv");
	if (!glUniform1uiv)
	{
		printf("Error: glUniform1uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform2uiv = (PFNGLUNIFORM2UIVPROC)wglGetProcAddress((const char*)"glUniform2uiv");
	if (!glUniform2uiv)
	{
		printf("Error: glUniform2uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform3uiv = (PFNGLUNIFORM3UIVPROC)wglGetProcAddress((const char*)"glUniform3uiv");
	if (!glUniform3uiv)
	{
		printf("Error: glUniform3uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glUniform4uiv = (PFNGLUNIFORM4UIVPROC)wglGetProcAddress((const char*)"glUniform4uiv");
	if (!glUniform4uiv)
	{
		printf("Error: glUniform4uiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)wglGetProcAddress((const char*)"glTexParameterIiv");
	if (!glTexParameterIiv)
	{
		printf("Error: glTexParameterIiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)wglGetProcAddress((const char*)"glTexParameterIuiv");
	if (!glTexParameterIuiv)
	{
		printf("Error: glTexParameterIuiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)wglGetProcAddress((const char*)"glGetTexParameterIiv");
	if (!glGetTexParameterIiv)
	{
		printf("Error: glGetTexParameterIiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)wglGetProcAddress((const char*)"glGetTexParameterIuiv");
	if (!glGetTexParameterIuiv)
	{
		printf("Error: glGetTexParameterIuiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)wglGetProcAddress((const char*)"glClearBufferiv");
	if (!glClearBufferiv)
	{
		printf("Error: glClearBufferiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)wglGetProcAddress((const char*)"glClearBufferuiv");
	if (!glClearBufferuiv)
	{
		printf("Error: glClearBufferuiv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)wglGetProcAddress((const char*)"glClearBufferfv");
	if (!glClearBufferfv)
	{
		printf("Error: glClearBufferfv function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)wglGetProcAddress((const char*)"glClearBufferfi");
	if (!glClearBufferfi)
	{
		printf("Error: glClearBufferfi function contained in OpenGL 3.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 0);
	}

	///////////////////////

	///////////////////////
	// OpenGL 3.1 Core 

	// GL_ARB_copy_buffer 
	if (IsExtensionSupported("GL_ARB_copy_buffer") == false)
	{
		printf("Error: Extension GL_ARB_copy_buffer is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)wglGetProcAddress((const char*)"glCopyBufferSubData");
		if (!glCopyBufferSubData)
		{
			printf("Error: glCopyBufferSubData function in GL_ARB_copy_buffer is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	// GL_ARB_uniform_buffer_object 
	if (IsExtensionSupported("GL_ARB_uniform_buffer_object") == false)
	{
		printf("Error: Extension GL_ARB_uniform_buffer_object is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)wglGetProcAddress((const char*)"glGetUniformIndices");
		if (!glGetUniformIndices)
		{
			printf("Error: glGetUniformIndices function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)wglGetProcAddress((const char*)"glGetActiveUniformsiv");
		if (!glGetActiveUniformsiv)
		{
			printf("Error: glGetActiveUniformsiv function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)wglGetProcAddress((const char*)"glGetActiveUniformName");
		if (!glGetActiveUniformName)
		{
			printf("Error: glGetActiveUniformName function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)wglGetProcAddress((const char*)"glGetUniformBlockIndex");
		if (!glGetUniformBlockIndex)
		{
			printf("Error: glGetUniformBlockIndex function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)wglGetProcAddress((const char*)"glGetActiveUniformBlockiv");
		if (!glGetActiveUniformBlockiv)
		{
			printf("Error: glGetActiveUniformBlockiv function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)wglGetProcAddress((const char*)"glGetActiveUniformBlockName");
		if (!glGetActiveUniformBlockName)
		{
			printf("Error: glGetActiveUniformBlockName function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

		glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)wglGetProcAddress((const char*)"glUniformBlockBinding");
		if (!glUniformBlockBinding)
		{
			printf("Error: glUniformBlockBinding function in GL_ARB_uniform_buffer_object is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)wglGetProcAddress((const char*)"glDrawArraysInstanced");
	if (!glDrawArraysInstanced)
	{
		printf("Error: glDrawArraysInstanced function contained in OpenGL 3.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 1);
	}

	glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)wglGetProcAddress((const char*)"glDrawElementsInstanced");
	if (!glDrawElementsInstanced)
	{
		printf("Error: glDrawElementsInstanced function contained in OpenGL 3.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 1);
	}

	glTexBuffer = (PFNGLTEXBUFFERPROC)wglGetProcAddress((const char*)"glTexBuffer");
	if (!glTexBuffer)
	{
		printf("Error: glTexBuffer function contained in OpenGL 3.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 1);
	}

	glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)wglGetProcAddress((const char*)"glPrimitiveRestartIndex");
	if (!glPrimitiveRestartIndex)
	{
		printf("Error: glPrimitiveRestartIndex function contained in OpenGL 3.1 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 1);
	}

	///////////////////////

	///////////////////////
	// OpenGL 3.2 Core 

	// GL_ARB_draw_elements_base_vertex 
	if (IsExtensionSupported("GL_ARB_draw_elements_base_vertex") == false)
	{
		printf("Error: Extension GL_ARB_draw_elements_base_vertex is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)wglGetProcAddress((const char*)"glDrawElementsBaseVertex");
		if (!glDrawElementsBaseVertex)
		{
			printf("Error: glDrawElementsBaseVertex function in GL_ARB_draw_elements_base_vertex is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)wglGetProcAddress((const char*)"glDrawRangeElementsBaseVertex");
		if (!glDrawRangeElementsBaseVertex)
		{
			printf("Error: glDrawRangeElementsBaseVertex function in GL_ARB_draw_elements_base_vertex is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)wglGetProcAddress((const char*)"glDrawElementsInstancedBaseVertex");
		if (!glDrawElementsInstancedBaseVertex)
		{
			printf("Error: glDrawElementsInstancedBaseVertex function in GL_ARB_draw_elements_base_vertex is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)wglGetProcAddress((const char*)"glMultiDrawElementsBaseVertex");
		if (!glMultiDrawElementsBaseVertex)
		{
			printf("Error: glMultiDrawElementsBaseVertex function in GL_ARB_draw_elements_base_vertex is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_provoking_vertex 
	if (IsExtensionSupported("GL_ARB_provoking_vertex") == false)
	{
		printf("Error: Extension GL_ARB_provoking_vertex is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)wglGetProcAddress((const char*)"glProvokingVertex");
		if (!glProvokingVertex)
		{
			printf("Error: glProvokingVertex function in GL_ARB_provoking_vertex is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_sync 
	if (IsExtensionSupported("GL_ARB_sync") == false)
	{
		printf("Error: Extension GL_ARB_sync is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glFenceSync = (PFNGLFENCESYNCPROC)wglGetProcAddress((const char*)"glFenceSync");
		if (!glFenceSync)
		{
			printf("Error: glFenceSync function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glIsSync = (PFNGLISSYNCPROC)wglGetProcAddress((const char*)"glIsSync");
		if (!glIsSync)
		{
			printf("Error: glIsSync function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glDeleteSync = (PFNGLDELETESYNCPROC)wglGetProcAddress((const char*)"glDeleteSync");
		if (!glDeleteSync)
		{
			printf("Error: glDeleteSync function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)wglGetProcAddress((const char*)"glClientWaitSync");
		if (!glClientWaitSync)
		{
			printf("Error: glClientWaitSync function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glWaitSync = (PFNGLWAITSYNCPROC)wglGetProcAddress((const char*)"glWaitSync");
		if (!glWaitSync)
		{
			printf("Error: glWaitSync function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glGetInteger64v = (PFNGLGETINTEGER64VPROC)wglGetProcAddress((const char*)"glGetInteger64v");
		if (!glGetInteger64v)
		{
			printf("Error: glGetInteger64v function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glGetSynciv = (PFNGLGETSYNCIVPROC)wglGetProcAddress((const char*)"glGetSynciv");
		if (!glGetSynciv)
		{
			printf("Error: glGetSynciv function in GL_ARB_sync is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_texture_multisample 
	if (IsExtensionSupported("GL_ARB_texture_multisample") == false)
	{
		printf("Error: Extension GL_ARB_texture_multisample is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress((const char*)"glTexImage2DMultisample");
		if (!glTexImage2DMultisample)
		{
			printf("Error: glTexImage2DMultisample function in GL_ARB_texture_multisample is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)wglGetProcAddress((const char*)"glTexImage3DMultisample");
		if (!glTexImage3DMultisample)
		{
			printf("Error: glTexImage3DMultisample function in GL_ARB_texture_multisample is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)wglGetProcAddress((const char*)"glGetMultisamplefv");
		if (!glGetMultisamplefv)
		{
			printf("Error: glGetMultisamplefv function in GL_ARB_texture_multisample is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

		glSampleMaski = (PFNGLSAMPLEMASKIPROC)wglGetProcAddress((const char*)"glSampleMaski");
		if (!glSampleMaski)
		{
			printf("Error: glSampleMaski function in GL_ARB_texture_multisample is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)wglGetProcAddress((const char*)"glGetInteger64i_v");
	if (!glGetInteger64i_v)
	{
		printf("Error: glGetInteger64i_v function contained in OpenGL 3.2 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}

	glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)wglGetProcAddress((const char*)"glGetBufferParameteri64v");
	if (!glGetBufferParameteri64v)
	{
		printf("Error: glGetBufferParameteri64v function contained in OpenGL 3.2 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}

	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress((const char*)"glFramebufferTexture");
	if (!glFramebufferTexture)
	{
		printf("Error: glFramebufferTexture function contained in OpenGL 3.2 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 2);
	}

	///////////////////////

	///////////////////////
	// OpenGL 3.3 Core 

	// GL_ARB_blend_func_extended 
	if (IsExtensionSupported("GL_ARB_blend_func_extended") == false)
	{
		printf("Error: Extension GL_ARB_blend_func_extended is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
		glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)wglGetProcAddress((const char*)"glBindFragDataLocationIndexed");
		if (!glBindFragDataLocationIndexed)
		{
			printf("Error: glBindFragDataLocationIndexed function in GL_ARB_blend_func_extended is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)wglGetProcAddress((const char*)"glGetFragDataIndex");
		if (!glGetFragDataIndex)
		{
			printf("Error: glGetFragDataIndex function in GL_ARB_blend_func_extended is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

	}

	// GL_ARB_sampler_objects 
	if (IsExtensionSupported("GL_ARB_sampler_objects") == false)
	{
		printf("Error: Extension GL_ARB_sampler_objects is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
		glGenSamplers = (PFNGLGENSAMPLERSPROC)wglGetProcAddress((const char*)"glGenSamplers");
		if (!glGenSamplers)
		{
			printf("Error: glGenSamplers function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)wglGetProcAddress((const char*)"glDeleteSamplers");
		if (!glDeleteSamplers)
		{
			printf("Error: glDeleteSamplers function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glIsSampler = (PFNGLISSAMPLERPROC)wglGetProcAddress((const char*)"glIsSampler");
		if (!glIsSampler)
		{
			printf("Error: glIsSampler function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glBindSampler = (PFNGLBINDSAMPLERPROC)wglGetProcAddress((const char*)"glBindSampler");
		if (!glBindSampler)
		{
			printf("Error: glBindSampler function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)wglGetProcAddress((const char*)"glSamplerParameteri");
		if (!glSamplerParameteri)
		{
			printf("Error: glSamplerParameteri function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)wglGetProcAddress((const char*)"glSamplerParameteriv");
		if (!glSamplerParameteriv)
		{
			printf("Error: glSamplerParameteriv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)wglGetProcAddress((const char*)"glSamplerParameterf");
		if (!glSamplerParameterf)
		{
			printf("Error: glSamplerParameterf function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)wglGetProcAddress((const char*)"glSamplerParameterfv");
		if (!glSamplerParameterfv)
		{
			printf("Error: glSamplerParameterfv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)wglGetProcAddress((const char*)"glSamplerParameterIiv");
		if (!glSamplerParameterIiv)
		{
			printf("Error: glSamplerParameterIiv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)wglGetProcAddress((const char*)"glSamplerParameterIuiv");
		if (!glSamplerParameterIuiv)
		{
			printf("Error: glSamplerParameterIuiv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)wglGetProcAddress((const char*)"glGetSamplerParameteriv");
		if (!glGetSamplerParameteriv)
		{
			printf("Error: glGetSamplerParameteriv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)wglGetProcAddress((const char*)"glGetSamplerParameterIiv");
		if (!glGetSamplerParameterIiv)
		{
			printf("Error: glGetSamplerParameterIiv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)wglGetProcAddress((const char*)"glGetSamplerParameterfv");
		if (!glGetSamplerParameterfv)
		{
			printf("Error: glGetSamplerParameterfv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)wglGetProcAddress((const char*)"glGetSamplerParameterIuiv");
		if (!glGetSamplerParameterIuiv)
		{
			printf("Error: glGetSamplerParameterIuiv function in GL_ARB_sampler_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

	}

	// GL_ARB_explicit_attrib_location, but it has none 
	if (IsExtensionSupported("GL_ARB_explicit_attrib_location") == false)
	{
		printf("Error: Extension GL_ARB_explicit_attrib_location, is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
	}

	// GL_ARB_occlusion_query2 (no entry points) 
	if (IsExtensionSupported("GL_ARB_occlusion_query2") == false)
	{
		printf("Error: Extension GL_ARB_occlusion_query2 is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
	}

	// GL_ARB_shader_bit_encoding (no entry points) 
	if (IsExtensionSupported("GL_ARB_shader_bit_encoding") == false)
	{
		printf("Error: Extension GL_ARB_shader_bit_encoding is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
	}

	// GL_ARB_texture_rgb10_a2ui (no entry points) 
	if (IsExtensionSupported("GL_ARB_texture_rgb10_a2ui") == false)
	{
		printf("Error: Extension GL_ARB_texture_rgb10_a2ui is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
	}

	// GL_ARB_texture_swizzle (no entry points) 
	if (IsExtensionSupported("GL_ARB_texture_swizzle") == false)
	{
		printf("Error: Extension GL_ARB_texture_swizzle is NOT supported (ignore this on AMD gpus)\n");

		// NOTE: AMD driver doesn't implement this one 
		//g_mMajor = min(g_mMajor, 3);
		//g_mMinor = min(g_mMinor, 3);
	}
	else
	{
	}

	// GL_ARB_timer_query 
	if (IsExtensionSupported("GL_ARB_timer_query") == false)
	{
		printf("Error: Extension GL_ARB_timer_query is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
		glQueryCounter = (PFNGLQUERYCOUNTERPROC)wglGetProcAddress((const char*)"glQueryCounter");
		if (!glQueryCounter)
		{
			printf("Error: glQueryCounter function in GL_ARB_timer_query is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)wglGetProcAddress((const char*)"glGetQueryObjecti64v");
		if (!glGetQueryObjecti64v)
		{
			printf("Error: glGetQueryObjecti64v function in GL_ARB_timer_query is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)wglGetProcAddress((const char*)"glGetQueryObjectui64v");
		if (!glGetQueryObjectui64v)
		{
			printf("Error: glGetQueryObjectui64v function in GL_ARB_timer_query is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

	}

	// GL_ARB_vertex_type_2_10_10_10_rev 
	if (IsExtensionSupported("GL_ARB_vertex_type_2_10_10_10_rev") == false)
	{
		printf("Error: Extension GL_ARB_vertex_type_2_10_10_10_rev is NOT supported\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}
	else
	{
		glVertexP2ui = (PFNGLVERTEXP2UIPROC)wglGetProcAddress((const char*)"glVertexP2ui");
		if (!glVertexP2ui)
		{
			printf("Error: glVertexP2ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)wglGetProcAddress((const char*)"glVertexP2uiv");
		if (!glVertexP2uiv)
		{
			printf("Error: glVertexP2uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexP3ui = (PFNGLVERTEXP3UIPROC)wglGetProcAddress((const char*)"glVertexP3ui");
		if (!glVertexP3ui)
		{
			printf("Error: glVertexP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)wglGetProcAddress((const char*)"glVertexP3uiv");
		if (!glVertexP3uiv)
		{
			printf("Error: glVertexP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexP4ui = (PFNGLVERTEXP4UIPROC)wglGetProcAddress((const char*)"glVertexP4ui");
		if (!glVertexP4ui)
		{
			printf("Error: glVertexP4ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)wglGetProcAddress((const char*)"glVertexP4uiv");
		if (!glVertexP4uiv)
		{
			printf("Error: glVertexP4uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)wglGetProcAddress((const char*)"glTexCoordP1ui");
		if (!glTexCoordP1ui)
		{
			printf("Error: glTexCoordP1ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)wglGetProcAddress((const char*)"glTexCoordP1uiv");
		if (!glTexCoordP1uiv)
		{
			printf("Error: glTexCoordP1uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)wglGetProcAddress((const char*)"glTexCoordP2ui");
		if (!glTexCoordP2ui)
		{
			printf("Error: glTexCoordP2ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)wglGetProcAddress((const char*)"glTexCoordP2uiv");
		if (!glTexCoordP2uiv)
		{
			printf("Error: glTexCoordP2uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)wglGetProcAddress((const char*)"glTexCoordP3ui");
		if (!glTexCoordP3ui)
		{
			printf("Error: glTexCoordP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)wglGetProcAddress((const char*)"glTexCoordP3uiv");
		if (!glTexCoordP3uiv)
		{
			printf("Error: glTexCoordP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)wglGetProcAddress((const char*)"glTexCoordP4ui");
		if (!glTexCoordP4ui)
		{
			printf("Error: glTexCoordP4ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)wglGetProcAddress((const char*)"glTexCoordP4uiv");
		if (!glTexCoordP4uiv)
		{
			printf("Error: glTexCoordP4uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)wglGetProcAddress((const char*)"glMultiTexCoordP1ui");
		if (!glMultiTexCoordP1ui)
		{
			printf("Error: glMultiTexCoordP1ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)wglGetProcAddress((const char*)"glMultiTexCoordP1uiv");
		if (!glMultiTexCoordP1uiv)
		{
			printf("Error: glMultiTexCoordP1uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)wglGetProcAddress((const char*)"glMultiTexCoordP2ui");
		if (!glMultiTexCoordP2ui)
		{
			printf("Error: glMultiTexCoordP2ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)wglGetProcAddress((const char*)"glMultiTexCoordP2uiv");
		if (!glMultiTexCoordP2uiv)
		{
			printf("Error: glMultiTexCoordP2uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)wglGetProcAddress((const char*)"glMultiTexCoordP3ui");
		if (!glMultiTexCoordP3ui)
		{
			printf("Error: glMultiTexCoordP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)wglGetProcAddress((const char*)"glMultiTexCoordP3uiv");
		if (!glMultiTexCoordP3uiv)
		{
			printf("Error: glMultiTexCoordP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)wglGetProcAddress((const char*)"glMultiTexCoordP4ui");
		if (!glMultiTexCoordP4ui)
		{
			printf("Error: glMultiTexCoordP4ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)wglGetProcAddress((const char*)"glMultiTexCoordP4uiv");
		if (!glMultiTexCoordP4uiv)
		{
			printf("Error: glMultiTexCoordP4uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glNormalP3ui = (PFNGLNORMALP3UIPROC)wglGetProcAddress((const char*)"glNormalP3ui");
		if (!glNormalP3ui)
		{
			printf("Error: glNormalP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glNormalP3uiv = (PFNGLNORMALP3UIVPROC)wglGetProcAddress((const char*)"glNormalP3uiv");
		if (!glNormalP3uiv)
		{
			printf("Error: glNormalP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glColorP3ui = (PFNGLCOLORP3UIPROC)wglGetProcAddress((const char*)"glColorP3ui");
		if (!glColorP3ui)
		{
			printf("Error: glColorP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glColorP3uiv = (PFNGLCOLORP3UIVPROC)wglGetProcAddress((const char*)"glColorP3uiv");
		if (!glColorP3uiv)
		{
			printf("Error: glColorP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glColorP4ui = (PFNGLCOLORP4UIPROC)wglGetProcAddress((const char*)"glColorP4ui");
		if (!glColorP4ui)
		{
			printf("Error: glColorP4ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glColorP4uiv = (PFNGLCOLORP4UIVPROC)wglGetProcAddress((const char*)"glColorP4uiv");
		if (!glColorP4uiv)
		{
			printf("Error: glColorP4uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)wglGetProcAddress((const char*)"glSecondaryColorP3ui");
		if (!glSecondaryColorP3ui)
		{
			printf("Error: glSecondaryColorP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)wglGetProcAddress((const char*)"glSecondaryColorP3uiv");
		if (!glSecondaryColorP3uiv)
		{
			printf("Error: glSecondaryColorP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)wglGetProcAddress((const char*)"glVertexAttribP1ui");
		if (!glVertexAttribP1ui)
		{
			printf("Error: glVertexAttribP1ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)wglGetProcAddress((const char*)"glVertexAttribP1uiv");
		if (!glVertexAttribP1uiv)
		{
			printf("Error: glVertexAttribP1uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)wglGetProcAddress((const char*)"glVertexAttribP2ui");
		if (!glVertexAttribP2ui)
		{
			printf("Error: glVertexAttribP2ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)wglGetProcAddress((const char*)"glVertexAttribP2uiv");
		if (!glVertexAttribP2uiv)
		{
			printf("Error: glVertexAttribP2uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)wglGetProcAddress((const char*)"glVertexAttribP3ui");
		if (!glVertexAttribP3ui)
		{
			printf("Error: glVertexAttribP3ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)wglGetProcAddress((const char*)"glVertexAttribP3uiv");
		if (!glVertexAttribP3uiv)
		{
			printf("Error: glVertexAttribP3uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)wglGetProcAddress((const char*)"glVertexAttribP4ui");
		if (!glVertexAttribP4ui)
		{
			printf("Error: glVertexAttribP4ui function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

		glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)wglGetProcAddress((const char*)"glVertexAttribP4uiv");
		if (!glVertexAttribP4uiv)
		{
			printf("Error: glVertexAttribP4uiv function in GL_ARB_vertex_type_2_10_10_10_rev is not supported!\n");
			g_mMajor = min(g_mMajor, 3);
			g_mMinor = min(g_mMinor, 3);
		}

	}

	glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)wglGetProcAddress((const char*)"glVertexAttribDivisor");
	if (!glVertexAttribDivisor)
	{
		printf("Error: glVertexAttribDivisor function contained in OpenGL 3.3 core is not supported!\n");
		g_mMajor = min(g_mMajor, 3);
		g_mMinor = min(g_mMinor, 3);
	}

	///////////////////////

	///////////////////////
	// OpenGL 4.0 Core 

	// GL_ARB_texture_query_lod (no entry points) 
	if (IsExtensionSupported("GL_ARB_texture_query_lod") == false)
	{
		printf("Error: Extension GL_ARB_texture_query_lod is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
	}

	// GL_ARB_draw_indirect 
	if (IsExtensionSupported("GL_ARB_draw_indirect") == false)
	{
		printf("Error: Extension GL_ARB_draw_indirect is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)wglGetProcAddress((const char*)"glDrawArraysIndirect");
		if (!glDrawArraysIndirect)
		{
			printf("Error: glDrawArraysIndirect function in GL_ARB_draw_indirect is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)wglGetProcAddress((const char*)"glDrawElementsIndirect");
		if (!glDrawElementsIndirect)
		{
			printf("Error: glDrawElementsIndirect function in GL_ARB_draw_indirect is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_gpu_shader5 (no entry points) 
	if (IsExtensionSupported("GL_ARB_gpu_shader5") == false)
	{
		printf("Error: Extension GL_ARB_gpu_shader5 is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
	}

	// GL_ARB_gpu_shader_fp64 
	if (IsExtensionSupported("GL_ARB_gpu_shader_fp64") == false)
	{
		printf("Error: Extension GL_ARB_gpu_shader_fp64 is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glUniform1d = (PFNGLUNIFORM1DPROC)wglGetProcAddress((const char*)"glUniform1d");
		if (!glUniform1d)
		{
			printf("Error: glUniform1d function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform2d = (PFNGLUNIFORM2DPROC)wglGetProcAddress((const char*)"glUniform2d");
		if (!glUniform2d)
		{
			printf("Error: glUniform2d function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform3d = (PFNGLUNIFORM3DPROC)wglGetProcAddress((const char*)"glUniform3d");
		if (!glUniform3d)
		{
			printf("Error: glUniform3d function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform4d = (PFNGLUNIFORM4DPROC)wglGetProcAddress((const char*)"glUniform4d");
		if (!glUniform4d)
		{
			printf("Error: glUniform4d function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform1dv = (PFNGLUNIFORM1DVPROC)wglGetProcAddress((const char*)"glUniform1dv");
		if (!glUniform1dv)
		{
			printf("Error: glUniform1dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform2dv = (PFNGLUNIFORM2DVPROC)wglGetProcAddress((const char*)"glUniform2dv");
		if (!glUniform2dv)
		{
			printf("Error: glUniform2dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform3dv = (PFNGLUNIFORM3DVPROC)wglGetProcAddress((const char*)"glUniform3dv");
		if (!glUniform3dv)
		{
			printf("Error: glUniform3dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniform4dv = (PFNGLUNIFORM4DVPROC)wglGetProcAddress((const char*)"glUniform4dv");
		if (!glUniform4dv)
		{
			printf("Error: glUniform4dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)wglGetProcAddress((const char*)"glUniformMatrix2dv");
		if (!glUniformMatrix2dv)
		{
			printf("Error: glUniformMatrix2dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)wglGetProcAddress((const char*)"glUniformMatrix3dv");
		if (!glUniformMatrix3dv)
		{
			printf("Error: glUniformMatrix3dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)wglGetProcAddress((const char*)"glUniformMatrix4dv");
		if (!glUniformMatrix4dv)
		{
			printf("Error: glUniformMatrix4dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)wglGetProcAddress((const char*)"glUniformMatrix2x3dv");
		if (!glUniformMatrix2x3dv)
		{
			printf("Error: glUniformMatrix2x3dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)wglGetProcAddress((const char*)"glUniformMatrix2x4dv");
		if (!glUniformMatrix2x4dv)
		{
			printf("Error: glUniformMatrix2x4dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)wglGetProcAddress((const char*)"glUniformMatrix3x2dv");
		if (!glUniformMatrix3x2dv)
		{
			printf("Error: glUniformMatrix3x2dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)wglGetProcAddress((const char*)"glUniformMatrix3x4dv");
		if (!glUniformMatrix3x4dv)
		{
			printf("Error: glUniformMatrix3x4dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)wglGetProcAddress((const char*)"glUniformMatrix4x2dv");
		if (!glUniformMatrix4x2dv)
		{
			printf("Error: glUniformMatrix4x2dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)wglGetProcAddress((const char*)"glUniformMatrix4x3dv");
		if (!glUniformMatrix4x3dv)
		{
			printf("Error: glUniformMatrix4x3dv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetUniformdv = (PFNGLGETUNIFORMDVPROC)wglGetProcAddress((const char*)"glGetUniformdv");
		if (!glGetUniformdv)
		{
			printf("Error: glGetUniformdv function in GL_ARB_gpu_shader_fp64 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_shader_subroutine 
	if (IsExtensionSupported("GL_ARB_shader_subroutine") == false)
	{
		printf("Error: Extension GL_ARB_shader_subroutine is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)wglGetProcAddress((const char*)"glGetSubroutineUniformLocation");
		if (!glGetSubroutineUniformLocation)
		{
			printf("Error: glGetSubroutineUniformLocation function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)wglGetProcAddress((const char*)"glGetSubroutineIndex");
		if (!glGetSubroutineIndex)
		{
			printf("Error: glGetSubroutineIndex function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)wglGetProcAddress((const char*)"glGetActiveSubroutineUniformiv");
		if (!glGetActiveSubroutineUniformiv)
		{
			printf("Error: glGetActiveSubroutineUniformiv function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)wglGetProcAddress((const char*)"glGetActiveSubroutineUniformName");
		if (!glGetActiveSubroutineUniformName)
		{
			printf("Error: glGetActiveSubroutineUniformName function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)wglGetProcAddress((const char*)"glGetActiveSubroutineName");
		if (!glGetActiveSubroutineName)
		{
			printf("Error: glGetActiveSubroutineName function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)wglGetProcAddress((const char*)"glUniformSubroutinesuiv");
		if (!glUniformSubroutinesuiv)
		{
			printf("Error: glUniformSubroutinesuiv function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)wglGetProcAddress((const char*)"glGetUniformSubroutineuiv");
		if (!glGetUniformSubroutineuiv)
		{
			printf("Error: glGetUniformSubroutineuiv function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)wglGetProcAddress((const char*)"glGetProgramStageiv");
		if (!glGetProgramStageiv)
		{
			printf("Error: glGetProgramStageiv function in GL_ARB_shader_subroutine is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_tessellation_shader 
	if (IsExtensionSupported("GL_ARB_tessellation_shader") == false)
	{
		printf("Error: Extension GL_ARB_tessellation_shader is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)wglGetProcAddress((const char*)"glPatchParameteri");
		if (!glPatchParameteri)
		{
			printf("Error: glPatchParameteri function in GL_ARB_tessellation_shader is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)wglGetProcAddress((const char*)"glPatchParameterfv");
		if (!glPatchParameterfv)
		{
			printf("Error: glPatchParameterfv function in GL_ARB_tessellation_shader is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_texture_buffer_object_rgb32 (no entry points) 
	if (IsExtensionSupported("GL_ARB_texture_buffer_object_rgb32") == false)
	{
		printf("Error: Extension GL_ARB_texture_buffer_object_rgb32 is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
	}

	// GL_ARB_texture_cube_map_array (no entry points) 
	if (IsExtensionSupported("GL_ARB_texture_cube_map_array") == false)
	{
		printf("Error: Extension GL_ARB_texture_cube_map_array is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
	}

	// GL_ARB_texture_gather (no entry points) 
	if (IsExtensionSupported("GL_ARB_texture_gather") == false)
	{
		printf("Error: Extension GL_ARB_texture_gather is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
	}

	// GL_ARB_transform_feedback2 
	if (IsExtensionSupported("GL_ARB_transform_feedback2") == false)
	{
		printf("Error: Extension GL_ARB_transform_feedback2 is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glBindTransformFeedback");
		if (!glBindTransformFeedback)
		{
			printf("Error: glBindTransformFeedback function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)wglGetProcAddress((const char*)"glDeleteTransformFeedbacks");
		if (!glDeleteTransformFeedbacks)
		{
			printf("Error: glDeleteTransformFeedbacks function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)wglGetProcAddress((const char*)"glGenTransformFeedbacks");
		if (!glGenTransformFeedbacks)
		{
			printf("Error: glGenTransformFeedbacks function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glIsTransformFeedback");
		if (!glIsTransformFeedback)
		{
			printf("Error: glIsTransformFeedback function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glPauseTransformFeedback");
		if (!glPauseTransformFeedback)
		{
			printf("Error: glPauseTransformFeedback function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glResumeTransformFeedback");
		if (!glResumeTransformFeedback)
		{
			printf("Error: glResumeTransformFeedback function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)wglGetProcAddress((const char*)"glDrawTransformFeedback");
		if (!glDrawTransformFeedback)
		{
			printf("Error: glDrawTransformFeedback function in GL_ARB_transform_feedback2 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	// GL_ARB_transform_feedback3 
	if (IsExtensionSupported("GL_ARB_transform_feedback3") == false)
	{
		printf("Error: Extension GL_ARB_transform_feedback3 is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}
	else
	{
		glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)wglGetProcAddress((const char*)"glDrawTransformFeedbackStream");
		if (!glDrawTransformFeedbackStream)
		{
			printf("Error: glDrawTransformFeedbackStream function in GL_ARB_transform_feedback3 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)wglGetProcAddress((const char*)"glBeginQueryIndexed");
		if (!glBeginQueryIndexed)
		{
			printf("Error: glBeginQueryIndexed function in GL_ARB_transform_feedback3 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)wglGetProcAddress((const char*)"glEndQueryIndexed");
		if (!glEndQueryIndexed)
		{
			printf("Error: glEndQueryIndexed function in GL_ARB_transform_feedback3 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

		glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)wglGetProcAddress((const char*)"glGetQueryIndexediv");
		if (!glGetQueryIndexediv)
		{
			printf("Error: glGetQueryIndexediv function in GL_ARB_transform_feedback3 is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 0);
		}

	}

	glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)wglGetProcAddress((const char*)"glMinSampleShading");
	if (!glMinSampleShading)
	{
		printf("Error: glMinSampleShading function contained in OpenGL 4.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}

	glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)wglGetProcAddress((const char*)"glBlendEquationi");
	if (!glBlendEquationi)
	{
		printf("Error: glBlendEquationi function contained in OpenGL 4.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}

	glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)wglGetProcAddress((const char*)"glBlendEquationSeparatei");
	if (!glBlendEquationSeparatei)
	{
		printf("Error: glBlendEquationSeparatei function contained in OpenGL 4.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}

	glBlendFunci = (PFNGLBLENDFUNCIPROC)wglGetProcAddress((const char*)"glBlendFunci");
	if (!glBlendFunci)
	{
		printf("Error: glBlendFunci function contained in OpenGL 4.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}

	glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)wglGetProcAddress((const char*)"glBlendFuncSeparatei");
	if (!glBlendFuncSeparatei)
	{
		printf("Error: glBlendFuncSeparatei function contained in OpenGL 4.0 core is not supported!\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 0);
	}

	///////////////////////

	///////////////////////
	// OpenGL 4.1 Core 

	// GL_ARB_ES2_compatibility 
	if (IsExtensionSupported("GL_ARB_ES2_compatibility") == false)
	{
		printf("Error: Extension GL_ARB_ES2_compatibility is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)wglGetProcAddress((const char*)"glReleaseShaderCompiler");
		if (!glReleaseShaderCompiler)
		{
			printf("Error: glReleaseShaderCompiler function in GL_ARB_ES2_compatibility is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glShaderBinary = (PFNGLSHADERBINARYPROC)wglGetProcAddress((const char*)"glShaderBinary");
		if (!glShaderBinary)
		{
			printf("Error: glShaderBinary function in GL_ARB_ES2_compatibility is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)wglGetProcAddress((const char*)"glGetShaderPrecisionFormat");
		if (!glGetShaderPrecisionFormat)
		{
			printf("Error: glGetShaderPrecisionFormat function in GL_ARB_ES2_compatibility is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glDepthRangef = (PFNGLDEPTHRANGEFPROC)wglGetProcAddress((const char*)"glDepthRangef");
		if (!glDepthRangef)
		{
			printf("Error: glDepthRangef function in GL_ARB_ES2_compatibility is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glClearDepthf = (PFNGLCLEARDEPTHFPROC)wglGetProcAddress((const char*)"glClearDepthf");
		if (!glClearDepthf)
		{
			printf("Error: glClearDepthf function in GL_ARB_ES2_compatibility is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	// GL_ARB_get_program_binary 
	if (IsExtensionSupported("GL_ARB_get_program_binary") == false)
	{
		printf("Error: Extension GL_ARB_get_program_binary is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)wglGetProcAddress((const char*)"glGetProgramBinary");
		if (!glGetProgramBinary)
		{
			printf("Error: glGetProgramBinary function in GL_ARB_get_program_binary is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramBinary = (PFNGLPROGRAMBINARYPROC)wglGetProcAddress((const char*)"glProgramBinary");
		if (!glProgramBinary)
		{
			printf("Error: glProgramBinary function in GL_ARB_get_program_binary is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)wglGetProcAddress((const char*)"glProgramParameteri");
		if (!glProgramParameteri)
		{
			printf("Error: glProgramParameteri function in GL_ARB_get_program_binary is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	// GL_ARB_separate_shader_objects 
	if (IsExtensionSupported("GL_ARB_separate_shader_objects") == false)
	{
		printf("Error: Extension GL_ARB_separate_shader_objects is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)wglGetProcAddress((const char*)"glUseProgramStages");
		if (!glUseProgramStages)
		{
			printf("Error: glUseProgramStages function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)wglGetProcAddress((const char*)"glActiveShaderProgram");
		if (!glActiveShaderProgram)
		{
			printf("Error: glActiveShaderProgram function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress((const char*)"glCreateShaderProgramv");
		if (!glCreateShaderProgramv)
		{
			printf("Error: glCreateShaderProgramv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)wglGetProcAddress((const char*)"glBindProgramPipeline");
		if (!glBindProgramPipeline)
		{
			printf("Error: glBindProgramPipeline function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)wglGetProcAddress((const char*)"glDeleteProgramPipelines");
		if (!glDeleteProgramPipelines)
		{
			printf("Error: glDeleteProgramPipelines function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)wglGetProcAddress((const char*)"glGenProgramPipelines");
		if (!glGenProgramPipelines)
		{
			printf("Error: glGenProgramPipelines function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)wglGetProcAddress((const char*)"glIsProgramPipeline");
		if (!glIsProgramPipeline)
		{
			printf("Error: glIsProgramPipeline function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)wglGetProcAddress((const char*)"glGetProgramPipelineiv");
		if (!glGetProgramPipelineiv)
		{
			printf("Error: glGetProgramPipelineiv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)wglGetProcAddress((const char*)"glProgramUniform1i");
		if (!glProgramUniform1i)
		{
			printf("Error: glProgramUniform1i function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)wglGetProcAddress((const char*)"glProgramUniform1iv");
		if (!glProgramUniform1iv)
		{
			printf("Error: glProgramUniform1iv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)wglGetProcAddress((const char*)"glProgramUniform1f");
		if (!glProgramUniform1f)
		{
			printf("Error: glProgramUniform1f function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)wglGetProcAddress((const char*)"glProgramUniform1fv");
		if (!glProgramUniform1fv)
		{
			printf("Error: glProgramUniform1fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)wglGetProcAddress((const char*)"glProgramUniform1d");
		if (!glProgramUniform1d)
		{
			printf("Error: glProgramUniform1d function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)wglGetProcAddress((const char*)"glProgramUniform1dv");
		if (!glProgramUniform1dv)
		{
			printf("Error: glProgramUniform1dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)wglGetProcAddress((const char*)"glProgramUniform1ui");
		if (!glProgramUniform1ui)
		{
			printf("Error: glProgramUniform1ui function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)wglGetProcAddress((const char*)"glProgramUniform1uiv");
		if (!glProgramUniform1uiv)
		{
			printf("Error: glProgramUniform1uiv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)wglGetProcAddress((const char*)"glProgramUniform2i");
		if (!glProgramUniform2i)
		{
			printf("Error: glProgramUniform2i function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)wglGetProcAddress((const char*)"glProgramUniform2iv");
		if (!glProgramUniform2iv)
		{
			printf("Error: glProgramUniform2iv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)wglGetProcAddress((const char*)"glProgramUniform2f");
		if (!glProgramUniform2f)
		{
			printf("Error: glProgramUniform2f function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)wglGetProcAddress((const char*)"glProgramUniform2fv");
		if (!glProgramUniform2fv)
		{
			printf("Error: glProgramUniform2fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)wglGetProcAddress((const char*)"glProgramUniform2d");
		if (!glProgramUniform2d)
		{
			printf("Error: glProgramUniform2d function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)wglGetProcAddress((const char*)"glProgramUniform2dv");
		if (!glProgramUniform2dv)
		{
			printf("Error: glProgramUniform2dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)wglGetProcAddress((const char*)"glProgramUniform2ui");
		if (!glProgramUniform2ui)
		{
			printf("Error: glProgramUniform2ui function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)wglGetProcAddress((const char*)"glProgramUniform2uiv");
		if (!glProgramUniform2uiv)
		{
			printf("Error: glProgramUniform2uiv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)wglGetProcAddress((const char*)"glProgramUniform3i");
		if (!glProgramUniform3i)
		{
			printf("Error: glProgramUniform3i function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)wglGetProcAddress((const char*)"glProgramUniform3iv");
		if (!glProgramUniform3iv)
		{
			printf("Error: glProgramUniform3iv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)wglGetProcAddress((const char*)"glProgramUniform3f");
		if (!glProgramUniform3f)
		{
			printf("Error: glProgramUniform3f function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)wglGetProcAddress((const char*)"glProgramUniform3fv");
		if (!glProgramUniform3fv)
		{
			printf("Error: glProgramUniform3fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)wglGetProcAddress((const char*)"glProgramUniform3d");
		if (!glProgramUniform3d)
		{
			printf("Error: glProgramUniform3d function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)wglGetProcAddress((const char*)"glProgramUniform3dv");
		if (!glProgramUniform3dv)
		{
			printf("Error: glProgramUniform3dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)wglGetProcAddress((const char*)"glProgramUniform3ui");
		if (!glProgramUniform3ui)
		{
			printf("Error: glProgramUniform3ui function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)wglGetProcAddress((const char*)"glProgramUniform3uiv");
		if (!glProgramUniform3uiv)
		{
			printf("Error: glProgramUniform3uiv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)wglGetProcAddress((const char*)"glProgramUniform4i");
		if (!glProgramUniform4i)
		{
			printf("Error: glProgramUniform4i function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)wglGetProcAddress((const char*)"glProgramUniform4iv");
		if (!glProgramUniform4iv)
		{
			printf("Error: glProgramUniform4iv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)wglGetProcAddress((const char*)"glProgramUniform4f");
		if (!glProgramUniform4f)
		{
			printf("Error: glProgramUniform4f function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)wglGetProcAddress((const char*)"glProgramUniform4fv");
		if (!glProgramUniform4fv)
		{
			printf("Error: glProgramUniform4fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)wglGetProcAddress((const char*)"glProgramUniform4d");
		if (!glProgramUniform4d)
		{
			printf("Error: glProgramUniform4d function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)wglGetProcAddress((const char*)"glProgramUniform4dv");
		if (!glProgramUniform4dv)
		{
			printf("Error: glProgramUniform4dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)wglGetProcAddress((const char*)"glProgramUniform4ui");
		if (!glProgramUniform4ui)
		{
			printf("Error: glProgramUniform4ui function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)wglGetProcAddress((const char*)"glProgramUniform4uiv");
		if (!glProgramUniform4uiv)
		{
			printf("Error: glProgramUniform4uiv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix2fv");
		if (!glProgramUniformMatrix2fv)
		{
			printf("Error: glProgramUniformMatrix2fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix3fv");
		if (!glProgramUniformMatrix3fv)
		{
			printf("Error: glProgramUniformMatrix3fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix4fv");
		if (!glProgramUniformMatrix4fv)
		{
			printf("Error: glProgramUniformMatrix4fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix2dv");
		if (!glProgramUniformMatrix2dv)
		{
			printf("Error: glProgramUniformMatrix2dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix3dv");
		if (!glProgramUniformMatrix3dv)
		{
			printf("Error: glProgramUniformMatrix3dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix4dv");
		if (!glProgramUniformMatrix4dv)
		{
			printf("Error: glProgramUniformMatrix4dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix2x3fv");
		if (!glProgramUniformMatrix2x3fv)
		{
			printf("Error: glProgramUniformMatrix2x3fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix3x2fv");
		if (!glProgramUniformMatrix3x2fv)
		{
			printf("Error: glProgramUniformMatrix3x2fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix2x4fv");
		if (!glProgramUniformMatrix2x4fv)
		{
			printf("Error: glProgramUniformMatrix2x4fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix4x2fv");
		if (!glProgramUniformMatrix4x2fv)
		{
			printf("Error: glProgramUniformMatrix4x2fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix3x4fv");
		if (!glProgramUniformMatrix3x4fv)
		{
			printf("Error: glProgramUniformMatrix3x4fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix4x3fv");
		if (!glProgramUniformMatrix4x3fv)
		{
			printf("Error: glProgramUniformMatrix4x3fv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix2x3dv");
		if (!glProgramUniformMatrix2x3dv)
		{
			printf("Error: glProgramUniformMatrix2x3dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix3x2dv");
		if (!glProgramUniformMatrix3x2dv)
		{
			printf("Error: glProgramUniformMatrix3x2dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix2x4dv");
		if (!glProgramUniformMatrix2x4dv)
		{
			printf("Error: glProgramUniformMatrix2x4dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix4x2dv");
		if (!glProgramUniformMatrix4x2dv)
		{
			printf("Error: glProgramUniformMatrix4x2dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix3x4dv");
		if (!glProgramUniformMatrix3x4dv)
		{
			printf("Error: glProgramUniformMatrix3x4dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)wglGetProcAddress((const char*)"glProgramUniformMatrix4x3dv");
		if (!glProgramUniformMatrix4x3dv)
		{
			printf("Error: glProgramUniformMatrix4x3dv function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)wglGetProcAddress((const char*)"glValidateProgramPipeline");
		if (!glValidateProgramPipeline)
		{
			printf("Error: glValidateProgramPipeline function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)wglGetProcAddress((const char*)"glGetProgramPipelineInfoLog");
		if (!glGetProgramPipelineInfoLog)
		{
			printf("Error: glGetProgramPipelineInfoLog function in GL_ARB_separate_shader_objects is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	// GL_ARB_shader_precision (no entry points) 
	if (IsExtensionSupported("GL_ARB_shader_precision") == false)
	{
		printf("Error: Extension GL_ARB_shader_precision is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
	}

	// GL_ARB_vertex_attrib_64bit 
	if (IsExtensionSupported("GL_ARB_vertex_attrib_64bit") == false)
	{
		printf("Error: Extension GL_ARB_vertex_attrib_64bit is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)wglGetProcAddress((const char*)"glVertexAttribL1d");
		if (!glVertexAttribL1d)
		{
			printf("Error: glVertexAttribL1d function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)wglGetProcAddress((const char*)"glVertexAttribL2d");
		if (!glVertexAttribL2d)
		{
			printf("Error: glVertexAttribL2d function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)wglGetProcAddress((const char*)"glVertexAttribL3d");
		if (!glVertexAttribL3d)
		{
			printf("Error: glVertexAttribL3d function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)wglGetProcAddress((const char*)"glVertexAttribL4d");
		if (!glVertexAttribL4d)
		{
			printf("Error: glVertexAttribL4d function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)wglGetProcAddress((const char*)"glVertexAttribL1dv");
		if (!glVertexAttribL1dv)
		{
			printf("Error: glVertexAttribL1dv function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)wglGetProcAddress((const char*)"glVertexAttribL2dv");
		if (!glVertexAttribL2dv)
		{
			printf("Error: glVertexAttribL2dv function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)wglGetProcAddress((const char*)"glVertexAttribL3dv");
		if (!glVertexAttribL3dv)
		{
			printf("Error: glVertexAttribL3dv function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)wglGetProcAddress((const char*)"glVertexAttribL4dv");
		if (!glVertexAttribL4dv)
		{
			printf("Error: glVertexAttribL4dv function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)wglGetProcAddress((const char*)"glVertexAttribLPointer");
		if (!glVertexAttribLPointer)
		{
			printf("Error: glVertexAttribLPointer function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)wglGetProcAddress((const char*)"glGetVertexAttribLdv");
		if (!glGetVertexAttribLdv)
		{
			printf("Error: glGetVertexAttribLdv function in GL_ARB_vertex_attrib_64bit is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	// GL_ARB_viewport_array 
	if (IsExtensionSupported("GL_ARB_viewport_array") == false)
	{
		printf("Error: Extension GL_ARB_viewport_array is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 1);
	}
	else
	{
		glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)wglGetProcAddress((const char*)"glViewportArrayv");
		if (!glViewportArrayv)
		{
			printf("Error: glViewportArrayv function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)wglGetProcAddress((const char*)"glViewportIndexedf");
		if (!glViewportIndexedf)
		{
			printf("Error: glViewportIndexedf function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)wglGetProcAddress((const char*)"glViewportIndexedfv");
		if (!glViewportIndexedfv)
		{
			printf("Error: glViewportIndexedfv function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glScissorArrayv = (PFNGLSCISSORARRAYVPROC)wglGetProcAddress((const char*)"glScissorArrayv");
		if (!glScissorArrayv)
		{
			printf("Error: glScissorArrayv function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)wglGetProcAddress((const char*)"glScissorIndexed");
		if (!glScissorIndexed)
		{
			printf("Error: glScissorIndexed function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)wglGetProcAddress((const char*)"glScissorIndexedv");
		if (!glScissorIndexedv)
		{
			printf("Error: glScissorIndexedv function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)wglGetProcAddress((const char*)"glDepthRangeArrayv");
		if (!glDepthRangeArrayv)
		{
			printf("Error: glDepthRangeArrayv function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)wglGetProcAddress((const char*)"glDepthRangeIndexed");
		if (!glDepthRangeIndexed)
		{
			printf("Error: glDepthRangeIndexed function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetFloati_v = (PFNGLGETFLOATI_VPROC)wglGetProcAddress((const char*)"glGetFloati_v");
		if (!glGetFloati_v)
		{
			printf("Error: glGetFloati_v function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

		glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)wglGetProcAddress((const char*)"glGetDoublei_v");
		if (!glGetDoublei_v)
		{
			printf("Error: glGetDoublei_v function in GL_ARB_viewport_array is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 1);
		}

	}

	///////////////////////

	///////////////////////
	// OpenGL 4.2 Core 

	// GL_ARB_base_instance 
	if (IsExtensionSupported("GL_ARB_base_instance") == false)
	{
		printf("Error: Extension GL_ARB_base_instance is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)wglGetProcAddress((const char*)"glDrawArraysInstancedBaseInstance");
		if (!glDrawArraysInstancedBaseInstance)
		{
			printf("Error: glDrawArraysInstancedBaseInstance function in GL_ARB_base_instance is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)wglGetProcAddress((const char*)"glDrawElementsInstancedBaseInstance");
		if (!glDrawElementsInstancedBaseInstance)
		{
			printf("Error: glDrawElementsInstancedBaseInstance function in GL_ARB_base_instance is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)wglGetProcAddress((const char*)"glDrawElementsInstancedBaseVertexBaseInstance");
		if (!glDrawElementsInstancedBaseVertexBaseInstance)
		{
			printf("Error: glDrawElementsInstancedBaseVertexBaseInstance function in GL_ARB_base_instance is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_shading_language_420pack (no entry points) 
	if (IsExtensionSupported("GL_ARB_shading_language_420pack") == false)
	{
		printf("Error: Extension GL_ARB_shading_language_420pack is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
	}

	// GL_ARB_transform_feedback_instanced 
	if (IsExtensionSupported("GL_ARB_transform_feedback_instanced") == false)
	{
		printf("Error: Extension GL_ARB_transform_feedback_instanced is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glDrawTransformFeedbackInstanced = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC)wglGetProcAddress((const char*)"glDrawTransformFeedbackInstanced");
		if (!glDrawTransformFeedbackInstanced)
		{
			printf("Error: glDrawTransformFeedbackInstanced function in GL_ARB_transform_feedback_instanced is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glDrawTransformFeedbackStreamInstanced = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC)wglGetProcAddress((const char*)"glDrawTransformFeedbackStreamInstanced");
		if (!glDrawTransformFeedbackStreamInstanced)
		{
			printf("Error: glDrawTransformFeedbackStreamInstanced function in GL_ARB_transform_feedback_instanced is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_compressed_texture_pixel_storage (no entry points) 
	if (IsExtensionSupported("GL_ARB_compressed_texture_pixel_storage") == false)
	{
		printf("Error: Extension GL_ARB_compressed_texture_pixel_storage is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
	}

	// GL_ARB_conservative_depth (no entry points) 
	if (IsExtensionSupported("GL_ARB_conservative_depth") == false)
	{
		printf("Error: Extension GL_ARB_conservative_depth is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
	}

	// GL_ARB_internalformat_query 
	if (IsExtensionSupported("GL_ARB_internalformat_query") == false)
	{
		printf("Error: Extension GL_ARB_internalformat_query is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC)wglGetProcAddress((const char*)"glGetInternalformativ");
		if (!glGetInternalformativ)
		{
			printf("Error: glGetInternalformativ function in GL_ARB_internalformat_query is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_map_buffer_alignment (no entry points) 
	if (IsExtensionSupported("GL_ARB_map_buffer_alignment") == false)
	{
		printf("Error: Extension GL_ARB_map_buffer_alignment is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
	}

	// GL_ARB_shader_atomic_counters 
	if (IsExtensionSupported("GL_ARB_shader_atomic_counters") == false)
	{
		printf("Error: Extension GL_ARB_shader_atomic_counters is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glGetActiveAtomicCounterBufferiv = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC)wglGetProcAddress((const char*)"glGetActiveAtomicCounterBufferiv");
		if (!glGetActiveAtomicCounterBufferiv)
		{
			printf("Error: glGetActiveAtomicCounterBufferiv function in GL_ARB_shader_atomic_counters is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_shader_image_load_store 
	if (IsExtensionSupported("GL_ARB_shader_image_load_store") == false)
	{
		printf("Error: Extension GL_ARB_shader_image_load_store is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)wglGetProcAddress((const char*)"glBindImageTexture");
		if (!glBindImageTexture)
		{
			printf("Error: glBindImageTexture function in GL_ARB_shader_image_load_store is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)wglGetProcAddress((const char*)"glMemoryBarrier");
		if (!glMemoryBarrier)
		{
			printf("Error: glMemoryBarrier function in GL_ARB_shader_image_load_store is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	// GL_ARB_shading_language_packing (no entry points) 
	if (IsExtensionSupported("GL_ARB_shading_language_packing") == false)
	{
		printf("Error: Extension GL_ARB_shading_language_packing is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
	}

	// GL_ARB_texture_storage 
	if (IsExtensionSupported("GL_ARB_texture_storage") == false)
	{
		printf("Error: Extension GL_ARB_texture_storage is NOT supported\n");
		g_mMajor = min(g_mMajor, 4);
		g_mMinor = min(g_mMinor, 2);
	}
	else
	{
		glTexStorage1D = (PFNGLTEXSTORAGE1DPROC)wglGetProcAddress((const char*)"glTexStorage1D");
		if (!glTexStorage1D)
		{
			printf("Error: glTexStorage1D function in GL_ARB_texture_storage is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress((const char*)"glTexStorage2D");
		if (!glTexStorage2D)
		{
			printf("Error: glTexStorage2D function in GL_ARB_texture_storage is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)wglGetProcAddress((const char*)"glTexStorage3D");
		if (!glTexStorage3D)
		{
			printf("Error: glTexStorage3D function in GL_ARB_texture_storage is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glTextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC)wglGetProcAddress((const char*)"glTextureStorage1DEXT");
		if (!glTextureStorage1DEXT)
		{
			printf("Error: glTextureStorage1DEXT function in GL_ARB_texture_storage is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glTextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC)wglGetProcAddress((const char*)"glTextureStorage2DEXT");
		if (!glTextureStorage2DEXT)
		{
			printf("Error: glTextureStorage2DEXT function in GL_ARB_texture_storage is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

		glTextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC)wglGetProcAddress((const char*)"glTextureStorage3DEXT");
		if (!glTextureStorage3DEXT)
		{
			printf("Error: glTextureStorage3DEXT function in GL_ARB_texture_storage is not supported!\n");
			g_mMajor = min(g_mMajor, 4);
			g_mMinor = min(g_mMinor, 2);
		}

	}

	///////////////////////

	// Now correct the major/minor version to correct one 
	switch (g_mMajor)
	{
	case 1:
		switch (g_mMinor)
		{
		case 3:
			g_mMajor = 1;
			g_mMinor = 0;
			break;

		case 4:
			g_mMajor = 1;
			g_mMinor = 3;
			break;

		case 5:
			g_mMajor = 1;
			g_mMinor = 4;
			break;
		}
		break;

	case 2:
		switch (g_mMinor)
		{
		case 0:
			g_mMajor = 1;
			g_mMinor = 5;
			break;

		case 1:
			g_mMajor = 2;
			g_mMinor = 0;
			break;
		}
		break;

	case 3:
		switch (g_mMinor)
		{
		case 0:
			g_mMajor = 2;
			g_mMinor = 1;
			break;

		case 1:
			g_mMajor = 3;
			g_mMinor = 0;
			break;

		case 2:
			g_mMajor = 3;
			g_mMinor = 1;
			break;

		case 3:
			g_mMajor = 3;
			g_mMinor = 2;
			break;
		}
		break;

	case 4:
		switch (g_mMinor)
		{
		case 0:
			g_mMajor = 3;
			g_mMinor = 3;
			break;

		case 1:
			g_mMajor = 4;
			g_mMinor = 0;
			break;

		case 2:
			g_mMajor = 4;
			g_mMinor = 1;
			break;

		case 3:
			g_mMajor = 4;
			g_mMinor = 2;
			break;
		}
		break;
	}

	free(g_mExtensionsList);

	*major = g_mMajor;
	*minor = g_mMinor;
}