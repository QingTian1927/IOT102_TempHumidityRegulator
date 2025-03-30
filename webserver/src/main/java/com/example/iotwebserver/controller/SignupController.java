package com.example.iotwebserver.controller;

import com.example.iotwebserver.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

@Controller
public class SignupController {
    @Autowired
    private UserService userService;

    @GetMapping(value = {"/signup"})
    public String signup() {
        return "signup";
    }

    @PostMapping(value = {"/save"})
    public String save(@RequestParam String username, @RequestParam String password, @RequestParam("repeat-password") String repeatPassword, Model model) {
        if(!password.equals(repeatPassword)) {
            model.addAttribute("error", "Passwords do not match!");
            return "signup";
        } else if (userService.registerUser(username, password)) {
            return "redirect:/login";
        } else {
            model.addAttribute("error", "Username is already taken!");
            return "signup";
        }
    }
}
