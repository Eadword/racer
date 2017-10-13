#include "shader.h"

#include <fstream>
using std::ifstream;
using std::ios;
using std::string;
#include <sstream>

#include <glm/gtc/type_ptr.hpp>


Shader::Shader() : m_programId(0) {}
Shader::~Shader() {
	destroy();
}

string Shader::getFileContents( const string & fileName )
{
	ifstream inFile( fileName.c_str(), ios::in );
	if( !inFile )
	{
		string msg = string("Unable to open shader file: ") + fileName;
		throw ShaderException(msg.c_str());
	}

	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();

	return code.str();
}

bool Shader::endsWith(const string & str, const string & end )
{
	if( str.size() < end.size() ) return false;
	string endStr = str.substr( str.size() - end.size(), end.size());
	return endStr == end;
}

void Shader::compileStageFile(const string & fileName)
{
	ShaderStage stage = VERTEX;

	if( endsWith(fileName, ".vert") || endsWith(fileName, "_vert.glsl") ||
		endsWith(fileName, ".vert.glsl") )
	{
		stage = VERTEX;
	}
	else if( endsWith(fileName, ".frag") || endsWith(fileName, "_frag.glsl") ||
		endsWith(fileName, ".frag.glsl") )
	{
		stage = FRAGMENT;
	}
	else if( endsWith(fileName, ".geom") || endsWith(fileName, "_geom.glsl") ||
		endsWith(fileName, ".geom.glsl") )
	{
		stage = GEOMETRY;
	}
	else if( endsWith(fileName, ".tcs") || endsWith(fileName, "_tcs.glsl") ||
		endsWith(fileName, ".tcs.glsl") )
	{
		stage = TESS_CONTROL;
	}
	else if( endsWith(fileName, ".tes") || endsWith(fileName, "_tes.glsl") ||
		endsWith(fileName, ".tes.glsl") )
	{
		stage = TESS_EVALUATION;
	}
	else
	{
		throw ShaderException("Unable to determine shader stage from file name");
	}

	compileStageFile(stage, fileName);
}

void Shader::compileStageFile( ShaderStage stage, const string & fileName)
{
	string code = getFileContents(fileName);
	compileStage(stage, code, fileName);
}

void Shader::compileStage( ShaderStage stage, const string & code, const string & fileName )
{
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	// Create the shader stage object
	GLuint stageId = gl->glCreateShader(stage);

	// Load the shader source
	const char * pCode = code.c_str();
	gl->glShaderSource(stageId, 1, &pCode, nullptr);

	// Compile
	gl->glCompileShader(stageId);

	// Check compilation status, if compilation failed, throw exception
	GLint status = 0;
	gl->glGetShaderiv(stageId, GL_COMPILE_STATUS, &status);
	if( GL_FALSE == status )
	{
		GLchar *log = nullptr;
		GLint logLen = 0;
		gl->glGetShaderiv(stageId, GL_INFO_LOG_LENGTH, &logLen);
		log = new GLchar[ logLen ];
		gl->glGetShaderInfoLog(stageId, logLen, nullptr, log);
		string logStr(log);
		delete [] log;

		gl->glDeleteShader(stageId);

		string msg = string("Failed to compile shader: ") + fileName + "\n";
		throw ShaderException(msg.c_str(), logStr );
	}
	else
	{
		if( m_programId == 0 )
		{
			m_programId = gl->glCreateProgram();
		}

		gl->glAttachShader(m_programId, stageId);
	}
}

void Shader::link( )
{
	if( m_programId == 0 )
	{
		throw ShaderException("Cannot link shader without any stages.");
	}

	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glLinkProgram( m_programId );

	GLint status = 0;
	gl->glGetProgramiv(m_programId, GL_LINK_STATUS, &status);
	if( GL_FALSE == status )
	{
		GLint logLen = 0;
		GLchar *log = nullptr;
		gl->glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &logLen);
		log = new GLchar[ logLen ];
		gl->glGetProgramInfoLog(m_programId, logLen, nullptr, log);
		string logStr(log);
		delete [] log;

		throw ShaderException( "Shader link failed: \n", logStr);
	}
}

void Shader::use()
{
	if( m_programId == 0 )
		throw ShaderException("Shader has not been compiled/linked.");

	QOpenGLFunctions_4_1_Core* gl =
    QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUseProgram(m_programId);
}

GLint Shader::getUniformLocation( const char * name )
{
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	return gl->glGetUniformLocation(m_programId, name);
}

void Shader::setUniform( const char * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUniform4f( getUniformLocation(name), x, y, z, w );
}

void Shader::setUniform( const char * name, GLfloat x, GLfloat y, GLfloat z ) {
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUniform3f( getUniformLocation(name), x, y, z );
}

void Shader::setUniform( const char * name, const glm::mat4 & m ) {
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUniformMatrix4fv( getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniform( const char * name, int val ) {
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUniform1i( getUniformLocation(name), val);
}

void Shader::setUniform( const char * name, const glm::vec4 & val) {
	setUniform(name, val.x, val.y, val.z, val.w);
}

void Shader::setUniform( const char * name, const glm::vec3 & val) {
	setUniform(name, val.x, val.y, val.z);
}

void Shader::setUniform( const char * name, const glm::mat3 & m) {
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUniformMatrix3fv( getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniform( const char * name, GLfloat val) {
	QOpenGLFunctions_4_1_Core* gl =
		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

	gl->glUniform1f( getUniformLocation(name), val);
}

void Shader::destroy()
{
	if( m_programId != 0 )
	{
		QOpenGLContext * ctx = QOpenGLContext::currentContext();
		if( ctx == nullptr ) return;

		QOpenGLFunctions_4_1_Core* gl = ctx->versionFunctions<QOpenGLFunctions_4_1_Core>();

		// Query the number of attached shaders
		GLint numShaders = 0;
		gl->glGetProgramiv( m_programId, GL_ATTACHED_SHADERS, &numShaders);

		// Get the shader names
		GLuint * shaderNames = new GLuint[numShaders];
		gl->glGetAttachedShaders(m_programId, numShaders, nullptr, shaderNames);

		// Delete the shaders
		for( int i = 0; i < numShaders; i++ )
			gl->glDeleteShader(shaderNames[i]);

		// Delete the program
		gl->glDeleteProgram(m_programId);
		m_programId = 0;

		delete [] shaderNames;
	}
}
