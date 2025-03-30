package com.example.iotwebserver.service;

import com.example.iotwebserver.model.User;
import com.example.iotwebserver.model.UserSettings;
import com.example.iotwebserver.repository.UserSettingsRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Optional;

@Service
public class UserSettingsService {

    private final UserSettingsRepository userSettingsRepository;

    @Autowired
    public UserSettingsService(UserSettingsRepository userSettingsRepository) {
        this.userSettingsRepository = userSettingsRepository;
    }

    public Optional<UserSettings> getUserSettings(User user) {
        return this.userSettingsRepository.findById(user.getId());
    }

    public UserSettings save(UserSettings userSettings) {
        return this.userSettingsRepository.save(userSettings);
    }
}
