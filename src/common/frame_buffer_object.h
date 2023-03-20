
#include <GL/glew.h>
#include <iostream>



struct frame_buffer_object {

	int w, h;
	GLuint id_fbo, id_tex, id_depth;

	void check(int fboStatus)
	{
		switch (fboStatus) {
		case GL_FRAMEBUFFER_COMPLETE:break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:          std::cout<<"FBO Incomplete: Attachment\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:  std::cout << "FBO Incomplete: Missing Attachment\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:      std::cout << "FBO Incomplete: Dimensions\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:         std::cout << "FBO Incomplete: Formats\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:         std::cout << "FBO Incomplete: Draw Buffer\n"; break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:         std::cout << "FBO Incomplete: Read Buffer\n"; break;
		case GL_FRAMEBUFFER_UNSUPPORTED:                    std::cout << "FBO Unsupported\n"; break;
		default:                                            std::cout << "Undefined FBO error\n"; break;
		}
	}


	void create(int w_, int h_)
	{
		if ((w == w_) && (h == h_))
			return;
		w = w_;
		h = h_;
		glActiveTexture(GL_TEXTURE0);

		glGenTextures(1, &this->id_tex);
		glBindTexture(GL_TEXTURE_2D, this->id_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glGenFramebuffers(1, &this->id_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, this->id_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->id_tex, 0);

		glGenRenderbuffers(1, &this->id_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, this->id_depth);
		glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->id_depth);

		int status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
		check(status);
		glBindFramebuffer (GL_FRAMEBUFFER, 0);
	}

	void remove()
	{
		glDeleteFramebuffers(1, &this->id_fbo);
		glDeleteRenderbuffers(1, &this->id_depth);
	}
};


