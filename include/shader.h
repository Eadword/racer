#pragma once

#include <QOpenGLFunctions_4_1_Core>
#include <glm/mat4x4.hpp>
#include <string>
#include <exception>

/// <summary>
/// This class encapsulates a GLSL shader program.  It can be used to compile,
/// link, and enable shader programs.  The following is an example of common
/// usage:
///
/// <code>
/// // One-time initialization
/// Shader s;
/// try {
///   s.compileStageFile("myshader.vert.glsl");
///   s.compileStageFile("myshader.frag.glsl");
///   s.link();
///   s.use();
/// }
/// catch(ShaderException & e) {
///   cerr << e.what() << endl;
///   cerr << e.getOpenGLLog() << endl;
///   ...
/// }
///
/// // Setting uniforms
/// s.setUniform( "MyUniform", 5.0f, 6.0f, 7.0f );
/// </code>
///
/// </summary>
class Shader
{
public:
	enum ShaderStage
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
	};

	/// <summary>
	/// Creates a Shader object.  Initially, this Shader is not bound to any
	/// OpenGL GLSL shader.  One must first call one of the compile functions,
	/// and link().
	/// </summary>
	Shader();

	/// <summary>
	///  Destroys the associated OpenGL shader object (if it exists).  Calls destroy().
	/// </summary>
	~Shader();

	/// <summary>
	/// Attempts to load and compile the given GLSL shader stage code located in
	/// the given file.  It attempts to determine the shader stage from the file
	/// name.  The file name must end with ".*.glsl", "_*.glsl", or
	/// ".*", where * is replaced by "vert", "frag", "geom", "tes", or "tcs".
	/// If not, the function fails and throws a ShaderException
	/// </summary>
	/// <param name="fileName">The name of the file containing the shader stage code.</param>
	void compileStageFile(const std::string & fileName);

	/// <summary>
	/// Attempts to load and compile the given GLSL shader stage code located in
	/// the given file.
	/// </summary>
	/// <param name="stage">The shader stage</param>
	/// <param name="fileName">The name of the file containing the shader stage code.</param>
	void compileStageFile(ShaderStage stage, const std::string & fileName);

	/// <summary>
	/// Attempts to compile the given GLSL shader stage code contained in the
	/// given std::string.
	/// </summary>
	/// <param name="stage">The shader stage</param>
	/// <param name="code">The shader stage code</param>
	/// <param name="fileName">If this code was loaded from a file, provide the
	///   file name here in order to provide better error messages.</param>
	void compileStage(ShaderStage stage, const std::string & code, const std::string &fileName = "");

	/// <summary>
	/// Attempts to link the associated shader program.  Do not call this method
	/// until you have at least compiled a fragment and vertex shader.
	/// </summary>
	void link();

	/// <summary>
	/// Load this shader program into the OpenGL pipeline (essentially calls
	///  glUseProgram).
	/// </summary>
	void use();

	/// <summary>
	/// Deletes the associated OpenGL program from OpenGL memory.
	/// </summary>
	void destroy();

	/// <summary>
	/// Set the given uniform variable to the given value.  This program must be
	/// loaded in the OpenGL pipeline before calling this function.
	/// </summary>
	/// <param name="name">The name of the uniform variable.  If this uniform
	///   variable does not exist, the method does nothing.</param>
	/// <param name="val">The value for the uniform variable</param>
	void setUniform( const char * name, const glm::vec4 & val);

	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, const glm::vec3 & val);
	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, GLfloat x, GLfloat y, GLfloat z);
	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, const glm::mat4 & value);
	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, const glm::mat3 & val);
	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, int val);
	/// <see>Shader::setUniform(const char*, const glm::vec4 & )</see>
	void setUniform( const char * name, GLfloat val);

private:
	std::string getFileContents( const std::string & fileName );
	bool endsWith( const std::string &, const std::string & );
	GLint getUniformLocation(const char *);

	GLuint m_programId;
};

/// <summary>
/// This exception is thrown when a shader fails to compile or link, or other
/// errors occur in the Shader class.
/// </summary>
class ShaderException : public std::exception
{
private:
	std::string m_log;
	std::string m_msg;
public:
	ShaderException(const char * msg, const std::string & log = "")
	{
		m_log = log;
		m_msg = msg;
	}
	virtual ~ShaderException() throw() {}

	/// <returns>
	/// The OpenGL log containing details about the failure.
	/// </returns>
	std::string getOpenGLLog() { return m_log; }

	/// <returns>An error message</returns>
	virtual const char* what() const throw() { return m_msg.c_str(); }
};
