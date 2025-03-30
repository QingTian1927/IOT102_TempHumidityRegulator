package com.example.iotwebserver.model.dto;

import com.example.iotwebserver.model.Reading;

public class ReadingUpload {
    private int temperature;
    private int humidity;

    public ReadingUpload(int temperature, int humidity) {
        this.temperature = temperature;
        this.humidity = humidity;
    }

    public Reading toReading() {
        return new Reading(temperature, humidity);
    }

    public int getTemperature() {
        return temperature;
    }

    public void setTemperature(int temperature) {
        this.temperature = temperature;
    }

    public int getHumidity() {
        return humidity;
    }

    public void setHumidity(int humidity) {
        this.humidity = humidity;
    }

    @Override
    public String toString() {
        return "ReadingUpload{" +
                "temperature=" + temperature +
                ", humidity=" + humidity +
                '}';
    }
}
