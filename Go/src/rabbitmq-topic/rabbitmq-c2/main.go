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

	// 声明连接通道
	ch, err := conn.Channel()
	failOnError(err, "Failed to open a channel")
	defer ch.Close()

	err = ch.ExchangeDeclare(
		"logs-topic", // name
		"topic",      // type
		true,         // durable
		false,        // auto-deleted
		false,        // internal
		false,        // no-wait
		nil,          // arguments
	)

	// 定义一个随机名队列
	q, err := ch.QueueDeclare(
		"",    // name，随机队列名
		false, // durable
		false, // delete when usused
		true,  // exclusive，独占模式，连接断开的时候，队列自动清除
		false, // no-wait
		nil,   // arguments
	)
	failOnError(err, "Failed to declare a queue")

	// 将随机名队列，绑定到logs exchange上去
	err = ch.QueueBind(
		q.Name,       // queue name
		"*.lazy.*",   // routing key
		"logs-topic", // exchange
		false,
		nil,
	)
	failOnError(err, "Failed to bind a queue")

	// 告诉rabbitmq-server给我们发消息，声明一下我们是消费者，向rabbitmq注册一下。不注册的话，rabbitmq是不会返回数据的。
	// 声明完了，会返回一个channel通道msgs，相当于建立了管道连接，后续会通过此管道给我们发数据
	msgs, err := ch.Consume(
		q.Name, // queue
		"",     // consumer
		true,   // auto-ack
		false,  // exclusive
		false,  // no-local
		false,  // no-wait
		nil,    // args
	)
	failOnError(err, "Failed to register a consumer")

	// 创建一个无缓存的channel，用来不死锁的卡住当前主线程的goroutine，防止主线程退出
	// 任何goroutine，都可以向此channel发送数据，用以退出当前主线程
	forever := make(chan bool)

	go func() {
		// 开始循环读取管道数据
		for d := range msgs {
			log.Printf("Received a message: %s", d.Body)
		}
	}()

	log.Printf(" [*] Waiting for messages. To exit press CTRL+C")
	// 无缓存，所以读会卡死，直到有写入数据
	<-forever
}
