#!/bin/bash

# Test script for the bookstore system

echo "Testing login with root"
echo -e "su root sjtu\nshow\nexit" | ./code

echo -e "\n\nTesting invalid login"
echo -e "su root wrongpassword\nexit" | ./code

echo -e "\n\nTesting register"
echo -e "register user1 pass1 User One\nsu user1 pass1\nexit" | ./code