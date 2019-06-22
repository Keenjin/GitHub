package main

import (
	"log"

	"github.com/streadway/amqp"
)

func failOnError(err error, msg string) {
	if err != nil {
		log.Fatalf("%s: %s", msg, err)
	}
}

func main() {
	// 与rabbitmq-server建立连接
	conn, err := amqp.Dial("amqp://keen-rabbit:keen123@192.168.0.100:5672/")
	failOnError(err, "Failed to connect to RabbitMQ")
	defer conn.Close()

	// channel是一条连接的任务抽象层，后续所有的操作，都是基于此连接的api的操作
	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	// 定义一个交换机，类型是fanout（广播类型），当publish的时候，可以广播消息到它所知道的队列（已bind队列）
	err = ch.ExchangeDeclare(
		"logs",   // name
		"fanout", // type
		true,     // durable	设置exchange为持久化的，为了防止消费者先run的话，bind会失败的问题
		false,    // auto-deleted
		false,    // internal
		false,    // no-wait
		nil,      // arguments
	)
	failOnError(err, "Failed to declare an exchange")

	// 发布消息
	body := "Hello World!"
	err = ch.Publish(
		"logs", // exchange
		"",     // routing key
		false,  // mandatory
		false,  // immediate
		amqp.Publishing{
			ContentType: "text/plain",
			Body:        []byte(body),
		})
	failOnError(err, "Failed to publish a message")
}
