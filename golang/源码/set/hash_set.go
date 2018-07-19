package set

import (
	"bytes"
	"fmt"
)

type HashSet struct {
	m map[interface{}]bool
}

func NewHashSet() *HashSet {
	return &HashSet{m: make(map[interface{}]bool)}
}

func (set *HashSet) Add(e interface{}) bool {
	if !set.m[e] {
		set.m[e] = true
		return true
	}
	return false
}
func (set *HashSet) Remove(e interface{}) {
	delete(set.m, e)

}
func (set *HashSet) Clear() {
	set.m = make(map[interface{}]bool)
}
func (set *HashSet) Len() int {
	return len(set.m)
}
func (set *HashSet) Contains(e interface{}) bool {
	return set.m[e]
}

func (set *HashSet) Same(other Set) bool {
	if other == nil {
		return false
	}
	if set.Len() != other.Len() {
		return false
	}
	for key := range set.m {
		if !other.Contains(key) {
			return false
		}
	}
	return true
}

// 生成 slice ,支持 for range 语句遍历
func (set *HashSet) Elements() []interface{} {
	initLen := len(set.m)

	snapshot := make([]interface{}, initLen)

	actualLen := 0
	// 当 在遍历时，set.m 添加新元素时，
	// 则  追加 到 生成的 slice 中
	for key := range set.m {
		if actualLen < initLen {
			snapshot[actualLen] = key
		} else {
			snapshot = append(snapshot, key)
		}
		actualLen++

	}
	//  当 遍历时， set.m 元素 被删除时
	// 对 生成的 slice 进行 分片
	if actualLen < initLen {
		snapshot = snapshot[:actualLen]
	}
	return snapshot
}

// to string
func (set *HashSet) String() string {
	var buff bytes.Buffer
	buff.WriteString("Set{")
	first := true
	for key := range set.m {
		if first {
			first = false
		} else {
			buff.WriteString("  ")
		}
		buff.WriteString(fmt.Sprintf("%v", key))
	}
	buff.WriteString("}")
	return buff.String()
}
