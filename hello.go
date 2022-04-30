package main

import (
	"fmt"
	"time"
)

var (
	l int
	m bool
)

const (
	x = iota
	y
	z
)

type Phone interface {
	call()
}

type NokiaPhone struct {
}

func (nokiaPhone NokiaPhone) call() {
	fmt.Println("I am Nokia, I can call you!")
}

type IPhone struct {
}

func (iPhone IPhone) call() {
	fmt.Println("I am IPhone, I can call you!")
}

func say(s string) {
	for i := 0; i < 5; i++ {
		time.Sleep(100 * time.Millisecond)
		fmt.Println(s)
	}
}

func sum(s []int, c chan int) {
	sum := 0
	for _, v := range s {
		sum += v
	}
	c <- sum
}

func main() {
	s := []int{7, 2, 8, -9, 4, 0}

	c := make(chan int)
	go sum(s[:len(s)/2], c)
	go sum(s[len(s)/2:], c)
	x, y := <-c, <-c

	fmt.Println(x, y, x+y)

	go say("World")
	say("hello")
	var phone Phone
	phone = new(NokiaPhone)
	phone.call()

	phone = new(IPhone)
	phone.call()

	var stockcode = 123
	var enddate = "2022-04-29"
	var url = "Code=%d&endDate=%s"
	var target_url = fmt.Sprintf(url, stockcode, enddate)
	fmt.Println(target_url)
	var a string = "Hello"
	var b, d int = 1, 2
	fmt.Println(a, b, d)
	f := "RRASSS"
	fmt.Println(f)
	println(l, m)
	const abc int = 1
	println(abc)
	println(x, y, z)
	var i = 0
	for i = 0; i < 100; i++ {
		println(i)
	}
	strings := []string{"google", "slackware"}
	for i, s := range strings {
		fmt.Println(i, s)
	}
	println(max(1, 2))

	s1, s2 := swap("google", "forward")
	println(s1, s2)

	var al int = 124
	var ah int = 456
	swapp(&al, &ah)
	println(al, ah)

	nextnum := getseq()
	println(nextnum())
	println(nextnum())
	println(nextnum())
	println(nextnum())

	nextnum2 := getseq()
	println(nextnum2())
	println(nextnum2())
	println(nextnum2())
	println(nextnum2())
	println(nextnum2())
	println(nextnum2())
	println(nextnum2())
	var c1 Circle
	c1.radius = 10.00
	println(c1.getArea())

	var book1 Books
	book1.title = "Hello"
	fmt.Println(book1)

	slice1 := make([]int, 10)

	fmt.Println(slice1)
}

type Books struct {
	title   string
	author  string
	subject string
	book_id int
}

type Circle struct {
	radius float64
}

func (c Circle) getArea() float64 {
	return 3.14 * c.radius * c.radius
}

func getseq() func() int {
	i := 0
	return func() int {
		i += 1
		return i
	}
}

func max(num1, num2 int) int {
	var result int

	if num1 > num2 {
		result = num1
	} else {
		result = num2
	}
	return result
}

func swap(x, y string) (string, string) {
	return y, x
}

func swapp(x *int, y *int) {
	var temp int
	temp = *x
	*x = *y
	*y = temp
}
