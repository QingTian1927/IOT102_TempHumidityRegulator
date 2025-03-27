package com.example.iotwebserver.model;

import jakarta.persistence.*;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.springframework.data.annotation.CreatedDate;

import java.util.Date;

@Entity
@Table(name = "readings")
@Data
@NoArgsConstructor
public class Reading {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private long id;

    @Column(name = "temperature", nullable = false)
    private int temperature;

    @Column(name = "humidity", nullable = false)
    private int humidity;

    @Column(name = "created_at", nullable = false)
    @CreatedDate
    private Date createdDate = new Date();
}
