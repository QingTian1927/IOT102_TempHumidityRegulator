package com.example.iotwebserver.controller;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class DashboardController {

    @GetMapping(value = {"/dashboard"})
    public String dashboard(Model model) {
        return "dashboard";
    }
}
