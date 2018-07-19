package set

type Set interface {
	Add(e interface{}) bool
	Remove(e interface{})
	Clear()
	Contains(e interface{}) bool
	Len() int
	Same(other Set) bool
	Elements() []interface{}
	String() string
}

func IsSuperset(one Set, other Set) bool {
	if one == nil || other == nil {
		return false
	}
	oneLen := one.Len()
	otherLen := other.Len()
	if oneLen == 0 || oneLen <= otherLen {
		return false
	}
	if oneLen > 0 && otherLen == 0 {
		return true
	}

	for _, v := range other.Elements() {
		if !one.Contains(v) {
			return false
		}
	}
	return true
}
func AddSet(one Set, other Set) {
	if one == nil || other == nil || other.Len() == 0 {
		return
	}

	for _, v := range other.Elements() {
		one.Add(v)
	}
}
func Union(one Set, other Set) Set {
	res := NewSimpleSet()
	AddSet(res, one)
	AddSet(res, other)
	return res
}

func Intersect(one Set, other Set) Set {

	res := NewSimpleSet()

	if one == nil || other == nil {
		return res
	}

	if one.Len() == 0 || other.Len() == 0 {
		return res
	}

	if one.Len() < other.Len() {
		for _, v := range one.Elements() {
			if other.Contains(v) {
				res.Add(v)
			}
		}
	} else {
		for _, v := range other.Elements() {
			if one.Contains(v) {
				res.Add(v)
			}
		}

	}
	return res
}

func Difference(one Set, other Set) Set {

	if one == nil {
		return nil
	}
	res := NewSimpleSet()

	if other == nil || other.Len() == 0 {
		AddSet(res, one)
		return res
	}

	for _, v := range one.Elements() {
		if !other.Contains(v) {
			res.Add(v)
		}
	}
	return res
}

func SymmetricDifference(one Set, other Set) Set {
	diffA := Difference(one, other)

	if other == nil || other.Len() == 0 {
		return diffA
	}
	diffB := Difference(other, one)
	return Union(diffA, diffB)
}

func NewSimpleSet() Set {
	return NewHashSet()
}

func IsSet(value interface{}) bool {
	if _, ok := value.(Set); ok {
		return true
	}
	return false
}
