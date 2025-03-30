package com.example.iotwebserver.model.dto;

import jakarta.persistence.Column;

public class UserSettingsForm {
    private String espIp;
    private int temperatureUpperBound;
    private int temperatureLowerBound;
    private int humidityUpperBound;
    private int humidityLowerBound;

    public UserSettingsForm(String espIp, int temperatureUpperBound, int temperatureLowerBound, int humidityUpperBound, int humidityLowerBound) {
        this.espIp = espIp;
        this.temperatureUpperBound = temperatureUpperBound;
        this.temperatureLowerBound = temperatureLowerBound;
        this.humidityUpperBound = humidityUpperBound;
        this.humidityLowerBound = humidityLowerBound;
    }

    public String getEspIp() {
        return espIp;
    }

    public void setEspIp(String espIp) {
        this.espIp = espIp;
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
        return "UserSettingsForm{" +
                "espIp='" + espIp + '\'' +
                ", temperatureUpperBound=" + temperatureUpperBound +
                ", temperatureLowerBound=" + temperatureLowerBound +
                ", humidityUpperBound=" + humidityUpperBound +
                ", humidityLowerBound=" + humidityLowerBound +
                '}';
    }
}
