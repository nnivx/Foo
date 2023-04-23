class BMFontInfo {
	construct New(map) {
		__face = map["face"]
		__size = map["size"]
		__bold = map["bold"]
		__italic = map["italic"]
		__charset = map["charset"]
		__stretchH = map["stretchH"]
		__smooth = map["smooth"]
		__aa = map["aa"]
		__padding = map["padding"]
		__spacing = map["spacing"]
		__outline = map["outline"]
	}

	face { __face }
	size { __size }
	bold { __bold }
	italic { __italic }
	charset { __charset }
	stretchH { __stretchH }
	smooth { __smooth }
	aa { __aa }
	padding { __padding[0..-1] }
	spacing { __spacing[0..-1] }
	outline { __outline }

}

import "io" for File
class BMFont {

	construct New(path) {
		var str = File.read(path)
		var lines = str.split("\n")

		var info = BMFont.read_info(lines[0])
		System.print(info)
		//for (line in lines) {
		//	var args = line.split(" ")
//
		//}
	}

	static split_space_but_not_when_quoted(str) {

	}

	static read_info(line) {
		var key_value_string = BMFont.split_space_but_not_when_quoted(line)
		var map = {}
		for (kvs in key_value_string[1..-1]) {
			var kv = split_kv(kvs)
			System.print(kv)
			map[kv[0]] = kv[1]
		}
		return map
	}

	static split_kv(str) {
		return str.split("=")
	}

}


var bmf = BMFont.New("C:\\Users\\nyksn\\source\\repos\\NyksFoo\\NyksFoo\\res\\fonts\\notosans.fnt")
