#ifndef MGL_FRAMEBUFFER_H_INCLUDED__
#define MGL_FRAMEBUFFER_H_INCLUDED__

template < typename type_t = unsigned int >
class mglFramebuffer
{
protected:
	type_t	*m_pixels;
	int		m_width;
	int		m_height;
private:
					mglFramebuffer(const mglFramebuffer&) {}
	mglFramebuffer	&operator=(const mglFramebuffer&) { return *this; }
public:
					mglFramebuffer( void ) : m_pixels(NULL), m_width(0), m_height(0) {}
	virtual			~mglFramebuffer( void ) { delete [] m_pixels; }
	virtual void	Create(int p_width, int p_height);
	virtual void	Free( void );
	void			Copy(const mglFramebuffer &p_buffer);
	bool			IsBad( void ) const { return m_pixels == NULL; }
	int				GetWidth( void ) const { return m_width; }
	int				GetHeight( void ) const { return m_height; }
	int				GetArea( void ) const { return m_width*m_height; }
	type_t			*GetPixels( void ) const { return m_pixels; }
	type_t			*GetPixels(int y) const { return m_pixels + y*m_width; }
	void			Fill(type_t value) const;
};

template < typename type_t >
void mglFramebuffer<type_t>::Create(int p_width, int p_height)
{
	Free();
	m_width = p_width;
	m_height = p_height;
	if (GetArea() > 0 && m_width > 0 && m_height > 0) {
		m_pixels = new type_t[GetArea()];
	}
}

template < typename type_t >
void mglFramebuffer<type_t>::Free( void )
{
	delete [] m_pixels;
	m_pixels = NULL;
	m_width = 0;
	m_height = 0;
}

template < typename type_t >
void mglFramebuffer<type_t>::Copy(const mglFramebuffer<type_t> &p_buffer)
{
	Create(p_buffer.m_width, p_buffer.m_height);
	for (int i = 0; i < GetArea(); ++i) {
		m_pixels[i] = p_buffer.m_pixels[i];
	}
}

template < typename type_t >
void mglFramebuffer<type_t>::Fill(type_t value) const
{
	const int area = GetArea();
	type_t *pixels = GetPixels();
	for (int i = 0; i < area; ++i) {
		pixels[i] = value;
	}
}

#endif
