package com.example.iotwebserver.repository;

import com.example.iotwebserver.model.Reading;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import java.util.List;
import java.util.Optional;

public interface ReadingRepository extends JpaRepository<Reading, Long> {
    @Query(value = "SELECT TOP 1 * FROM readings ORDER BY created_at DESC", nativeQuery = true)
    Optional<Reading> getLatestReading();

    @Query(value = """
            SELECT
                DATEPART(HOUR, created_at) AS hour,
                AVG(humidity) AS averageHumidity,
                AVG(temperature) AS averageTemperature
            FROM readings
            WHERE CAST(created_at AS DATE) = CAST(GETDATE() AS DATE)
            GROUP BY DATEPART(HOUR, created_at)
            ORDER BY hour
            """, nativeQuery = true)
    List<Object[]> getHourlyAverages();

    @Query(value = """
            SELECT
                FORMAT(created_at, 'yyyy-MM-dd') AS day,
                AVG(humidity) AS averageHumidity,
                AVG(temperature) AS averageTemperature
            FROM readings
            WHERE DATEPART(WEEK, created_at) = DATEPART(WEEK, GETDATE())
              AND YEAR(created_at) = YEAR(GETDATE())
            GROUP BY FORMAT(created_at, 'yyyy-MM-dd')
            ORDER BY day
            """, nativeQuery = true)
    List<Object[]> getDailyAveragesForWeek();

    @Query(value = """
            SELECT
                CONCAT('Week ', DATEPART(WEEK, created_at)) AS week,
                AVG(humidity) AS averageHumidity,
                AVG(temperature) AS averageTemperature
            FROM readings
            WHERE DATEPART(MONTH, created_at) = DATEPART(MONTH, GETDATE())
              AND YEAR(created_at) = YEAR(GETDATE())
            GROUP BY DATEPART(WEEK, created_at)
            ORDER BY DATEPART(WEEK, created_at)
            """, nativeQuery = true)
    List<Object[]> getWeeklyAveragesForMonth();
}
