package com.example.iotwebserver.controller;

import com.example.iotwebserver.model.User;
import com.example.iotwebserver.model.UserSettings;
import com.example.iotwebserver.model.dto.UserSettingsForm;
import com.example.iotwebserver.service.UserService;
import com.example.iotwebserver.service.UserSettingsService;
import jakarta.servlet.http.HttpSession;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;

import java.util.Optional;

@Controller
public class DashboardController {
    private final UserService userService;
    private final UserSettingsService userSettingsService;

    @Autowired
    public DashboardController(UserService userService, UserSettingsService userSettingsService) {
        this.userService = userService;
        this.userSettingsService = userSettingsService;
    }

    @GetMapping("/")
    public String index() {
        return "redirect:/dashboard";
    }

    @GetMapping(value = {"/dashboard"})
    public String dashboard(Model model, HttpSession session) {
        String username = (String) session.getAttribute("username");
        Optional<User> user = userService.findByUsername(username);

        if (user.isEmpty()) {
            return "redirect:/login";
        }

        Optional<UserSettings> userSettings = userSettingsService.getUserSettings(user.get());
        userSettings.ifPresent(settings -> model.addAttribute("settings", settings));

        return "dashboard";
    }

    @PostMapping("/settings")
    public String saveSettings(@ModelAttribute UserSettingsForm params, HttpSession session) {
        System.out.println(params);

        String username = (String) session.getAttribute("username");
        Optional<User> user = userService.findByUsername(username);

        if (user.isEmpty()) {
            return "redirect:/login";
        }

        Optional<UserSettings> userSettings = userSettingsService.getUserSettings(user.get());
        if (userSettings.isEmpty()) {
            userSettings = Optional.of(this.userSettingsService.save(new UserSettings(user.get())));
        }

        if (params.getEspIp() != null) {
            userSettings.get().setEspIP(params.getEspIp());
        }
        if (params.getHumidityLowerBound() != null) {
            userSettings.get().setHumidityLowerBound(params.getHumidityLowerBound());
        }
        if (params.getHumidityUpperBound() != null) {
            userSettings.get().setHumidityUpperBound(params.getHumidityUpperBound());
        }
        if (params.getTemperatureLowerBound() != null) {
            userSettings.get().setTemperatureLowerBound(params.getTemperatureLowerBound());
        }
        if (params.getTemperatureUpperBound() != null) {
            userSettings.get().setTemperatureUpperBound(params.getTemperatureUpperBound());
        }

        this.userSettingsService.save(userSettings.get());
        return "redirect:/dashboard";
    }
}
