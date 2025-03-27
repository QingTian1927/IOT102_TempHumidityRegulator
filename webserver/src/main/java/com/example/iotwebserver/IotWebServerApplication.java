package com.example.iotwebserver;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.data.jpa.repository.config.EnableJpaAuditing;

@SpringBootApplication
@EnableJpaAuditing
public class IotWebServerApplication {

    public static void main(String[] args) {
        SpringApplication.run(IotWebServerApplication.class, args);
    }

}
