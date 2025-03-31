package com.example.iotwebserver.model.dto;

public class UserSettingsForm {
    private String espIp;
    private Integer temperatureUpperBound;
    private Integer temperatureLowerBound;
    private Integer humidityUpperBound;
    private Integer humidityLowerBound;

    public UserSettingsForm(String espIp, Integer temperatureUpperBound, Integer temperatureLowerBound, Integer humidityUpperBound, Integer humidityLowerBound) {
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

    public Integer getTemperatureUpperBound() {
        return temperatureUpperBound;
    }

    public void setTemperatureUpperBound(Integer temperatureUpperBound) {
        this.temperatureUpperBound = temperatureUpperBound;
    }

    public Integer getTemperatureLowerBound() {
        return temperatureLowerBound;
    }

    public void setTemperatureLowerBound(Integer temperatureLowerBound) {
        this.temperatureLowerBound = temperatureLowerBound;
    }

    public Integer getHumidityUpperBound() {
        return humidityUpperBound;
    }

    public void setHumidityUpperBound(Integer humidityUpperBound) {
        this.humidityUpperBound = humidityUpperBound;
    }

    public Integer getHumidityLowerBound() {
        return humidityLowerBound;
    }

    public void setHumidityLowerBound(Integer humidityLowerBound) {
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
