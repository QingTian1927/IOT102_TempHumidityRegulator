package com.example.iotwebserver.controller;

import com.example.iotwebserver.model.Reading;
import com.example.iotwebserver.model.dto.ReadingUpload;
import com.example.iotwebserver.service.ReadingService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.ArrayList;
import java.util.List;

@RestController
@RequestMapping("/api")
public class ApiController {

    public static final String STATISTICS_HOURLY = "hourly";
    public static final String STATISTICS_DAILY = "daily";
    public static final String STATISTICS_WEEKLY = "weekly";

    private final ReadingService readingService;

    @Autowired
    public ApiController(ReadingService readingService) {
        this.readingService = readingService;
    }

    @PostMapping("/upload")
    public void uploadReading(@RequestBody ReadingUpload readingUpload) {
        Reading reading = readingUpload.toReading();
        this.readingService.save(reading);
    }

    @GetMapping("/statistics/{type}")
    public List<Object[]> getStatistics(@PathVariable String type) {
        switch (type) {
            case STATISTICS_HOURLY -> {
                return readingService.getHourlyAverages();
            }
            case STATISTICS_DAILY -> {
                return readingService.getDailyAveragesForWeek();
            }
            case STATISTICS_WEEKLY -> {
                return readingService.getWeeklyAveragesForMonth();
            }
            default -> {
                return new ArrayList<>();
            }
        }
    }
}
