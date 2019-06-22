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

	// 定义一个队列，用于发送消息。注意，只有队列不存在的时候，才会创建队列。如果队列存在，则获取该队列
	q, err := ch.QueueDeclare(
		"hello", // name
		false,   // durable
		false,   // delete when unused
		false,   // exclusive
		false,   // no-wait
		nil,     // arguments
	)
	failOnError(err, "Failed to declare a queue")

	// 发布消息
	body := "Hello World!"
	err = ch.Publish(
		"",     // exchange
		q.Name, // routing key
		false,  // mandatory
		false,  // immediate
		amqp.Publishing{
			DeliveryMode: amqp.Persistent,
			ContentType:  "text/plain",
			Body:         []byte(body),
		})
	failOnError(err, "Failed to publish a message")
}
