package com.example.iotwebserver.controller;

import com.example.iotwebserver.service.UserService;
import jakarta.servlet.http.HttpSession;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

@Controller
public class LoginController {
    @Autowired
    private UserService userService;

    @GetMapping(value = {"/login"})
    public String login(Model model) {
        return "login";
    }

    @PostMapping(value = {"/login"})
    public String processLogin(@RequestParam String username, @RequestParam String password, HttpSession session, Model model) {
        if (userService.checkLogin(username, password)) {
            session.setAttribute("user", username);
            return "redirect:/dashboard";
        } else {
            model.addAttribute("error", "Invalid username or password!");
            return "login";
        }
    }
}
