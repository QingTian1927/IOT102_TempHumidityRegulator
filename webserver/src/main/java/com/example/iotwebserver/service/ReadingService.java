package com.example.iotwebserver.service;

import com.example.iotwebserver.model.Reading;
import com.example.iotwebserver.repository.ReadingRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Optional;

@Service
public class ReadingService {

    private final ReadingRepository readingRepository;

    @Autowired
    public ReadingService(ReadingRepository readingRepository) {
        this.readingRepository = readingRepository;
    }

    public Optional<Reading> getLatestReading() {
        return this.readingRepository.getLatestReading();
    }

    public Reading save(Reading reading) {
        Optional<Reading> latestReading = this.readingRepository.getLatestReading();
        if (latestReading.isEmpty()) {
            return this.readingRepository.save(reading);
        }

        int temperature = latestReading.get().getTemperature();
        int humidity = latestReading.get().getHumidity();

        if (temperature == reading.getTemperature() && humidity == reading.getHumidity()) {
            return null;
        }

        return this.readingRepository.save(reading);
    }
}
