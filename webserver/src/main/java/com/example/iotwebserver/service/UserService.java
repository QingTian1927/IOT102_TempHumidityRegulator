package com.example.iotwebserver.service;

import com.example.iotwebserver.model.User;
import com.example.iotwebserver.model.UserSettings;
import com.example.iotwebserver.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import java.util.Optional;

@Service
public class UserService {
    private final UserRepository userRepository;
    private final UserSettingsService userSettingsService;

    @Autowired
    public UserService(UserRepository userRepository, UserSettingsService userSettingsService) {
        this.userRepository = userRepository;
        this.userSettingsService = userSettingsService;
    }

    public Optional<User> findByUsername(String username) {
        return this.userRepository.findByUsername(username);
    }

    public boolean checkLogin(String username, String password) {
        Optional<User> user = userRepository.findByUsername(username);
        return user.isPresent() && user.get().getPassword().equals(password);
    }


    public boolean registerUser(String username, String password) {
        if (userRepository.findByUsername(username).isPresent()) {
            return false; // Người dùng đã tồn tại
        }
        User user = new User(username, password);
        user = userRepository.save(user);

        System.out.println(user);

        UserSettings userSettings = new UserSettings(user);
        System.out.println(userSettings);

        userSettingsService.save(userSettings);
        return true;
    }
}
