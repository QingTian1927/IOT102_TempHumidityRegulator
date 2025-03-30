package com.example.iotwebserver.controller;

import com.example.iotwebserver.model.Reading;
import com.example.iotwebserver.model.dto.ReadingUpload;
import com.example.iotwebserver.service.ReadingService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("api")
public class ApiController {

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
}
