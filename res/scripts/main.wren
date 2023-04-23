System.print("Hello SMORT people, from Wren! so basically i can do what i want with this here now")

foreign class Point {
	construct new(x, y) {
		System.print("here in wren")
	}
}

class Maths {
	static lerp(a, b, v) {
		return a*v + b*(1-v)
	}
}

var p = Point.new(5, 3)

class State {

	construct new(pos, vel) {
		_pos = pos
		_vel = vel
		_accel = 0
	}
	
	pos { _pos }
	pos=(v) { _pos = v }

	vel { _vel }
	vel=(v) { _vel = v }

	accel { _accel }
	accel=(v) { _accel = v }

	integrate(dt, r) {
		r.vel = vel
		r.pos = pos
		r.accel = accel

		vel = vel + accel*dt
		pos = pos + vel*dt
	}

	blend(a, b, v) {
		pos = Maths.lerp(a.pos, a.pos, v)
		vel = Maths.lerp(a.vel, a.vel, v)
	}

}

// Entry Point
class Game {

	static init() {
		__prev = State.new(0, 0)
		__cur = State.new(0, 0)
		__render = State.new(0, 0)
	}

	// dt = microseconds
	static integrate(dt) {
		__cur.integrate(dt, __prev)
		//System.print("dt=%(dt):\tposition = %(__cur.pos)")
	}

	static blend(a) {
		__render.blend(__cur, __prev, a)
	}

}

Game.init()

var vert_shader = 
"""
#version 330 core
layout (location = 0) in vec3 pos;

void main() {
	gl_Position = vec4(pos.xyz, 1.0);
}
"""

var frag_shader =
"""
#version 330 core
out vec4 FragColor;

void main() {
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
"""
