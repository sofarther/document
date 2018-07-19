package main

import (
	"fmt"
	"reflect"
	"sort"
)

//排序接口
type Sortable interface {
	sort.Interface
	Sort()
}
//扩展 排序接口 
// 元素类型 使用 interface{} 类型，这样可接受任何类型
type GenericSeq interface {
	Sortable
	Append(e interface{}) bool
	Set(index int, e interface{}) bool
	Delete(index int) (interface{}, bool)
	ElemValue(index int) interface{}
	ElemType() reflect.Type //获取 元素实际类型
	Value() interface{}
}

//自定义类型 实现 GenericSeq 接口

type SortableSlice []interface{}


func (self SortableSlice) Len() int {
	return len(self)
}
// sort.Less() 方法中 当元素的类型为 interface{}时，则Go默认推导为  struct
// 因此当 直接 使用 < 进行判断元素大小时，在编译时将直接报错
//即使 实现类型 为 int float32 string 等支持 < 操作的类型
//因此需对类型进行区分判断处理
func (self SortableSlice) Less(i, j int) bool {
	//var v1 interface{} = self[i]
	//var v2 interface{} = self[j]
	//t1 := reflect.ValueOf(v1)
	//t2 := reflect.ValueOf(v2)
	//fmt.Println(t1)
	//fmt.Println(t2)
	//	fmt.Println(reflect.TypeOf(self[i]))
	//return true
	//return t1 < t2
	//return self[i] < self[j]
	return self.Compare(i, j)
}
func (self SortableSlice) Compare(i, j int) bool {
	t1 := reflect.ValueOf(self[i])
	t2 := reflect.ValueOf(self[j])
	k := t1.Kind()
	//针对不同 动态类型 进行转化实际类型，并处理
	if k >= reflect.Int && k <= reflect.Uint64 {
		v1 := t1.Int()
		v2 := t2.Int()
		return v1 < v2
	}
	if k == reflect.Float32 || k == reflect.Float64 {
		v1 := t1.Float()
		v2 := t2.Float()
		return v1 < v2

	}

	if k == reflect.String {

		v1 := t1.String()
		v2 := t2.String()
		return v1 < v2
	}

	return true
}
func (self SortableSlice) Swap(i, j int) {
	self[i], self[j] = self[j], self[i]
}
func (this SortableSlice) Sort() {
	//fmt.Println("Sort")
	sort.Sort(this)
}
//由于 会修改 self 本身值，因此 需要  将 指针类型 作为接受者
func (self *SortableSlice) Append(e interface{}) bool {
	*self = append(*self, e)

	return true
}

func (self SortableSlice) Set(index int, e interface{}) bool {
	if index < 0 || index >= len(self) {
		return false
	}
	self[index] = e
	return true
}
func (self *SortableSlice) Delete(index int) (interface{}, bool) {
	l := len(*self)
	if index < 0 || index >= l {
		return nil, false
	}
	res := (*self)[index]
	for i := index; i < l-1; i++ {
		(*self)[i] = (*self)[i+1]
	}
	*self = (*self)[:l-1]
	return res, true
}
func (self SortableSlice) ElemValue(index int) interface{} {
	if index < 0 || index >= len(self) {
		return false
	}
	return self[index]

}
func (self SortableSlice) ElemType() reflect.Type {
	return reflect.TypeOf(self[0])
}
func (self SortableSlice) Value() interface{} {
	return self
}
// 实现 GenericSeq 接口类型
type Sequence struct {
	GenericSeq
	sorted   bool
	elemType reflect.Type
}

func (self *Sequence) Sort() {
	if !self.sorted {
		self.GenericSeq.Sort()
		self.sorted = true

	}

}
func (self *Sequence) Append(e interface{}) bool {
	result := self.GenericSeq.Append(e)
	self.sorted = false

	return result
}

func (self *Sequence) Set(index int, e interface{}) bool {
	result := self.GenericSeq.Set(index, e)
	self.sorted = false

	return result

}
func (self *Sequence) ElemType() reflect.Type {
	if self.elemType == nil {
		self.elemType = self.GenericSeq.ElemType()
	}
	return self.elemType

}
func intTest() {

	var ss = SortableSlice{34, 23, 53, 15, 25}
	var t int
	//由于SortableSlice 类型 在实现 GenericSeq 接口时 有使用  *SortableSlice 作为接受者方法
   //因此 GenericSeq字段 需使用 SortableSlice 指针类型 赋值
	seq := Sequence{GenericSeq: &ss, elemType: reflect.TypeOf(t)}
	seq.Sort()
	fmt.Printf("%v\n", seq)
}
func floatTest() {
	var ss = SortableSlice{34.3, 23.2, 53.5, 15.3, 25.5, 23.1, 15.42}
	var t float32
	seq := Sequence{GenericSeq: &ss, elemType: reflect.TypeOf(t)}
	seq.Append(31.2)
	seq.Delete(5)
	seq.Set(3, 42.7)
	fmt.Printf("%v\n", seq.GenericSeq) // 输出SortableSlice 全部元素，并开始处带有& 标示
	seq.Sort()
	fmt.Printf("%v\n", seq.GenericSeq)

}
func stringTest() {
	var ss = SortableSlice{"10", "5", "23", "42", "18", "3"}
	var t string
	seq := Sequence{GenericSeq: &ss, elemType: reflect.TypeOf(t)}
	seq.Sort()
	fmt.Printf("%v\n", seq.GenericSeq)

}
func main() {
	floatTest()
	stringTest()
}
