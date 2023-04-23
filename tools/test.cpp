#include <iostream>

struct HiddenBase {
	void call() const {
		std::cout << "Hidden\n";
	}
};

struct PublicInterface: private HiddenBase {
	void call() const {
		std::cout << "From Public\n";
		HiddenBase::call();
	}
};

struct ShouldBeSimple {
	int a, b;

	ShouldBeSimple(int a, int b)
		: a{a}, b{b} {

	}

	constexpr ShouldBeSimple() noexcept = default;
};

template <class Pred, class T>
	requires (
		requires (Pred p) {
			{ p(std::declval<T>(), std::declval<T>()) } -> std::same_as<bool>;
		}
	)
void sort_sprites(Pred pred, T a, T b) {
	std::cout << std::boolalpha;
	std::cout << pred(a, b) << std::endl;
}

bool less(int a, int b) { return a < b; }

#include <vector>

#include <filesystem>

namespace Design1 {
	enum class EventType: int {
		Key,
	};

	template <EventType E>
	struct EventBase {
		static constexpr inline EventType type = E;
	};

	struct KeyEvent: EventBase<EventType::Key> {

	};
}


enum class FunnyNumber: int {
	SixtyNine,
	Fortwenny,
};

struct hmm {
	union {
		float sixtynine;
		int fortwenny;
	};
	FunnyNumber type;
	bool constant_evaluated;
	int extra_flags;
};

template <class>
inline constexpr bool always_false = false;

template <class... Args>
constexpr hmm make_me_hmm(const FunnyNumber N, Args&&... args) noexcept {
	static_assert(sizeof...(Args) == 1, "Yo not possible bro");

	hmm re;
	re.type = N;
	switch (N) {
		using enum FunnyNumber;
		case SixtyNine: re.sixtynine = float(std::forward<Args>(args)...); break;
		case Fortwenny: re.fortwenny = int (std::forward<Args>(args)...); break;
	}
	re.constant_evaluated = std::is_constant_evaluated();

	re.extra_flags = sizeof...(Args);
	return re;
}

constexpr int is_this_possible(FunnyNumber v) noexcept {
	switch (v) {
		using enum FunnyNumber;
		case SixtyNine: return 69;
		case Fortwenny: return 420;
	}
	return -1;
}

static constexpr inline int some_constexpr_value = is_this_possible(FunnyNumber::SixtyNine);

static constexpr inline hmm hmmmmm = make_me_hmm(FunnyNumber::SixtyNine, 69);

#define CAT(z) "what" # z \
	

std::ostream& operator<<(std::ostream& out, const hmm& v) {
	if (v.constant_evaluated)
		out << "constexpr ";
	out << "hmm(";
	out << int(v.type);
	out << ", ";
	switch (v.type) {
		case FunnyNumber::SixtyNine: out << v.sixtynine; break;
		case FunnyNumber::Fortwenny: out << v.fortwenny; break;
	}
	out << ')';
	out << ' ' << v.extra_flags;
	return out;
}

int buffer_[2]{0, 1};
bool current_buffer_ = true;

struct subsystem {
	subsystem() {
		std::cout << "Creating subsystem" << std::endl;
	}
	~subsystem() {
		std::cout << "Deleting subsystem" << std::endl;
	}
};

struct mainsystem {
	int foo, bar;
	subsystem sub;
	mainsystem(int i)
		: foo(i)
		, bar(foo - 7) {
		std::cout << "Creating main " << foo << " " << bar << std::endl;
	}
	~mainsystem() {
		std::cout << "Deleting main" << std::endl;
	}
};

int main(int argc, char** argv) {
	mainsystem azxczxca(4);
	std::cout << buffer_[(current_buffer_ = !current_buffer_)] << std::endl;
	std::cout << buffer_[(current_buffer_ = !current_buffer_)] << std::endl;
	std::cout << buffer_[(current_buffer_ = !current_buffer_)] << std::endl;

	std::cout << CAT(yoh) << std::endl;
	std::cout << std::boolalpha;
	PublicInterface obj;
	obj.call();

	std::vector<int> ints{0, 1, 2, 3, 4};
//	auto v = ints.insert(ints.end(), {5, 6, 7});
//	std::cout << *v << std::endl;
	auto v = ints.insert(ints.end(), 4, 100);
	std::cout << (v - ints.begin()) << std::endl;

	std::cout << std::is_trivially_constructible_v<hmm> << std::endl;

	std::cout << some_constexpr_value << std::endl;

	std::cout << hmmmmm << '\n';
	hmm a = make_me_hmm(FunnyNumber::Fortwenny, 12);
	std::cout << a << '\n';

	int x = 5;
	a = make_me_hmm(FunnyNumber::SixtyNine, 12);
	std::cout << a << '\n';
	sort_sprites([](int a, int b) {
		return a < b;
	}, 1, 2);



	return 0;
}