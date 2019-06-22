package main

import (
	"bytes"
	"encoding/base64"
	"fmt"
	"log"
	"unicode/utf8"
)

func main() {
	str := `
	a
	b
	c
	d
	中国
	`
	fmt.Println(str)

	str = "aaa"
	fmt.Println(len(str))
	fmt.Println(utf8.RuneCountInString(str))

	fmt.Println()

	str = "aaa中国"
	fmt.Println(len(str))
	fmt.Println(utf8.RuneCountInString(str))

	fmt.Println()

	for i := 0; i < len(str); i++ {
		fmt.Printf("%c", str[i])
	}

	fmt.Println()

	for _, s := range str {
		fmt.Printf("%c", s)
	}

	fmt.Println()

	str_change := []byte(str)
	str_change[0] = 'b'
	fmt.Println(string(str_change))

	hammer := "锤子"
	sssss := "aaaaa"
	new_str := hammer + sssss
	fmt.Println(new_str)
	var new_str2 bytes.Buffer
	new_str2.WriteString(hammer)
	new_str2.WriteString(sssss)
	fmt.Println(new_str2.String())

	// base64
	msg := base64.StdEncoding.EncodeToString([]byte(hammer))
	fmt.Println(msg)
	data, err := base64.StdEncoding.DecodeString(msg)
	if err != nil {
		log.Fatal(err)
	} else {
		fmt.Println(string(data))
	}
}
