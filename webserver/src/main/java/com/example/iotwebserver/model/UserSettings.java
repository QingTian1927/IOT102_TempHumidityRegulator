package com.example.iotwebserver.model;

import jakarta.persistence.*;

import java.util.UUID;

@Entity
@Table(name = "user_settings")
public class UserSettings {
    public static final int TEMPERATURE_HARD_UPPER_BOUND = 50;
    public static final int TEMPERATURE_HARD_LOWER_BOUND = 0;
    public static final int HUMIDITY_HARD_UPPER_BOUND = 80;
    public static final int HUMIDITY_HARD_LOWER_BOUND = 20;

    @Id
    @Column(name = "user_id")
    private UUID id;

    @OneToOne(cascade = CascadeType.ALL)
    @JoinColumn(name = "user_id", nullable = false)
    private User user;

    @Column(name = "esp_ip")
    private String espIP;

    @Column(name = "temp_upper_bound")
    private int temperatureUpperBound = TEMPERATURE_HARD_UPPER_BOUND;

    @Column(name = "temp_lower_bound")
    private int temperatureLowerBound = TEMPERATURE_HARD_LOWER_BOUND;

    @Column(name = "humidity_upper_bound")
    private int humidityUpperBound = HUMIDITY_HARD_UPPER_BOUND;

    @Column(name = "humidity_lower_bound")
    private int humidityLowerBound = HUMIDITY_HARD_LOWER_BOUND;

    public UserSettings() {
    }

    public UserSettings(User user) {
        this.user = user;
        this.id = user.getId();
    }

    public UUID getId() {
        return id;
    }

    public void setId(UUID id) {
        this.id = id;
    }

    public User getUser() {
        return user;
    }

    public void setUser(User user) {
        this.user = user;
    }

    public String getEspIP() {
        return espIP;
    }

    public void setEspIP(String espIP) {
        this.espIP = espIP;
    }

    public int getTemperatureUpperBound() {
        return temperatureUpperBound;
    }

    public void setTemperatureUpperBound(int temperatureUpperBound) {
        this.temperatureUpperBound = temperatureUpperBound;
    }

    public int getTemperatureLowerBound() {
        return temperatureLowerBound;
    }

    public void setTemperatureLowerBound(int temperatureLowerBound) {
        this.temperatureLowerBound = temperatureLowerBound;
    }

    public int getHumidityUpperBound() {
        return humidityUpperBound;
    }

    public void setHumidityUpperBound(int humidityUpperBound) {
        this.humidityUpperBound = humidityUpperBound;
    }

    public int getHumidityLowerBound() {
        return humidityLowerBound;
    }

    public void setHumidityLowerBound(int humidityLowerBound) {
        this.humidityLowerBound = humidityLowerBound;
    }

    @Override
    public String toString() {
        return "UserSettings{" +
                "id=" + id +
                ", user=" + user +
                ", espIP='" + espIP + '\'' +
                ", temperatureUpperBound=" + temperatureUpperBound +
                ", temperatureLowerBound=" + temperatureLowerBound +
                ", humidityUpperBound=" + humidityUpperBound +
                ", humidityLowerBound=" + humidityLowerBound +
                '}';
    }
}
