package com.example.iotwebserver.repository;

import com.example.iotwebserver.model.Reading;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.Optional;

public interface ReadingRepository extends JpaRepository<Reading, Long> {
    @Query(value = "SELECT TOP 1 * FROM readings ORDER BY created_at DESC", nativeQuery = true)
    Optional<Reading> getLatestReading();
}
