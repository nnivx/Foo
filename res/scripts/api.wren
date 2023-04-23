
// Can't be used until main(!!)
// Only expose or pragmatically import after entrypoint
class Render {

	static render_text(text, r, g, b, x, y, sx, sy) {

	}

}

class Foo {

	static version_string(str) {
		var major = Num.fromString(str[0])
		var minor = Num.fromString(str[2])

		var s = "%(major)%(minor)0"

		if (major >= 3 && minor >= 3) {
			return s
		}

		var v = {
			"200": "110",
			"210": "120",
			"300": "130",
			"310": "140",
			"320": "150",
		}[s]

		return v? v: null
	}

	static version_string_header(str) {
		var v = version_string(str)
		return v? "#version %(v)": null
	}

}

System.print(Foo.version_string_header("1.4"))
System.print(Foo.version_string_header("2.3"))
System.print(Foo.version_string_header("3.3.2"))
System.print(Foo.version_string_header("3.2"))
System.print(Foo.version_string_header("4.6.6"))



var fmt = "
GL_R8 	GL_RED 	8
GL_R8_SNORM 	GL_RED 	s8
GL_R16 	GL_RED 	16
GL_R16_SNORM 	GL_RED 	s16
GL_RG8 	GL_RG 	8 	8
GL_RG8_SNORM 	GL_RG 	s8 	s8
GL_RG16 	GL_RG 	16 	16
GL_RG16_SNORM 	GL_RG 	s16 	s16
GL_R3_G3_B2 	GL_RGB 	3 	3 	2
GL_RGB4 	GL_RGB 	4 	4 	4
GL_RGB5 	GL_RGB 	5 	5 	5
GL_RGB8 	GL_RGB 	8 	8 	8
GL_RGB8_SNORM 	GL_RGB 	s8 	s8 	s8
GL_RGB10 	GL_RGB 	10 	10 	10
GL_RGB12 	GL_RGB 	12 	12 	12
GL_RGB16_SNORM 	GL_RGB 	16 	16 	16
GL_RGBA2 	GL_RGB 	2 	2 	2 	2
GL_RGBA4 	GL_RGB 	4 	4 	4 	4
GL_RGB5_A1 	GL_RGBA 	5 	5 	5 	1
GL_RGBA8 	GL_RGBA 	8 	8 	8 	8
GL_RGBA8_SNORM 	GL_RGBA 	s8 	s8 	s8 	s8
GL_RGB10_A2 	GL_RGBA 	10 	10 	10 	2
GL_RGB10_A2UI 	GL_RGBA 	ui10 	ui10 	ui10 	ui2
GL_RGBA12 	GL_RGBA 	12 	12 	12 	12
GL_RGBA16 	GL_RGBA 	16 	16 	16 	16
GL_SRGB8 	GL_RGB 	8 	8 	8
GL_SRGB8_ALPHA8 	GL_RGBA 	8 	8 	8 	8
GL_R16F 	GL_RED 	f16
GL_RG16F 	GL_RG 	f16 	f16
GL_RGB16F 	GL_RGB 	f16 	f16 	f16
GL_RGBA16F 	GL_RGBA 	f16 	f16 	f16 	f16
GL_R32F 	GL_RED 	f32
GL_RG32F 	GL_RG 	f32 	f32
GL_RGB32F 	GL_RGB 	f32 	f32 	f32
GL_RGBA32F 	GL_RGBA 	f32 	f32 	f32 	f32
GL_R11F_G11F_B10F 	GL_RGB 	f11 	f11 	f10
GL_RGB9_E5 	GL_RGB 	9 	9 	9 		5
GL_R8I 	GL_RED 	i8
GL_R8UI 	GL_RED 	ui8
GL_R16I 	GL_RED 	i16
GL_R16UI 	GL_RED 	ui16
GL_R32I 	GL_RED 	i32
GL_R32UI 	GL_RED 	ui32
GL_RG8I 	GL_RG 	i8 	i8
GL_RG8UI 	GL_RG 	ui8 	ui8
GL_RG16I 	GL_RG 	i16 	i16
GL_RG16UI 	GL_RG 	ui16 	ui16
GL_RG32I 	GL_RG 	i32 	i32
GL_RG32UI 	GL_RG 	ui32 	ui32
GL_RGB8I 	GL_RGB 	i8 	i8 	i8
GL_RGB8UI 	GL_RGB 	ui8 	ui8 	ui8
GL_RGB16I 	GL_RGB 	i16 	i16 	i16
GL_RGB16UI 	GL_RGB 	ui16 	ui16 	ui16
GL_RGB32I 	GL_RGB 	i32 	i32 	i32
GL_RGB32UI 	GL_RGB 	ui32 	ui32 	ui32
GL_RGBA8I 	GL_RGBA 	i8 	i8 	i8 	i8
GL_RGBA8UI 	GL_RGBA 	ui8 	ui8 	ui8 	ui8
GL_RGBA16I 	GL_RGBA 	i16 	i16 	i16 	i16
GL_RGBA16UI 	GL_RGBA 	ui16 	ui16 	ui16 	ui16
GL_RGBA32I 	GL_RGBA 	i32 	i32 	i32 	i32
GL_RGBA32UI 	GL_RGBA
"

var clean = Fn.new { |input|
	
}
System.print(fmt)