package main

import (
	"fmt"
	wasm "github.com/wasmerio/wasmer-go/wasmer"
	"os/exec"
	"log"
)


func main() {
	bytes, _ := wasm.ReadBytes("main.wasm")

	instance, _ := wasm.NewInstance(bytes)
	defer instance.Close()
	init := instance.Exports["info"]
	result,_ := init()
	f := result.String()
	if (f != "1") {
	        fmt.Println("Not ready to deploy")
	} else {
	        fmt.Println("Ready to deploy")
	        out, err := exec.Command("/bin/sh", "deploy.sh").Output()
	        if err != nil {
		                log.Fatal(err)
	        }
	        fmt.Println(string(out))
	}
}
