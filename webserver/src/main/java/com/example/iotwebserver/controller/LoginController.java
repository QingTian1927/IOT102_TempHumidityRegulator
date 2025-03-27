package com.example.iotwebserver.controller;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;

@Controller
public class LoginController {

    @GetMapping(value = {"/login"})
    public String login(Model model) {
        return "login";
    }

    @PostMapping(value = {"/login"})
    public String processLogin(Model model) {
        return "login";
    }
}
