#9.Jan
#update password is from password.txt from outside of file
#projects should be selected.

#where the team_id come from?



# 1.Imports
from bs4 import BeautifulSoup
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException, NoSuchElementException

from datetime import datetime
from dateutil.parser import parse
import time

# Set up the Chrome WebDriver

#2.Setup Chrome WebDriver:
#This line creates a ChromeOptions object, 
#which allows you to set various options for the Chrome driver.
#options = webdriver.ChromeOptions()


chrome_options = Options()
chrome_options.add_experimental_option("debuggerAddress", "localhost:65536")

#This option runs Chrome in headless mode, 
#it will not display a UI or open a browser window.

########options.add_argument("--headless")  # Run in headless mode, without a UI.

#3.Initialize WebDriver:
#This line initializes a new instance of Chrome with the specified options.
driver = webdriver.Chrome(options=chrome_options)
#driver = webdriver.Chrome('/home/hongbaki/chromedriver', options=chrome_options)

# Navigate to the login page and log in
# 4.Login Sequence:

# ----------------------------------------
# Assume login has already been done.

# #The script tells the browser to load the login page.
# driver.get("https://profile.intra.42.fr/login")
# #Selenium finds the username input field by its ID and types the username into it.
# driver.find_element(By.ID, "user_login").send_keys("your_username")
# #Similarly, it finds the password field and enters the password.
# driver.find_element(By.ID, "user_password").send_keys("your_password")
# #It finds the submit button for the form and clicks it to log in.
# driver.find_element(By.NAME, "commit").click()
# -----------------------------------------








# Replace these with your actual login credentials and element IDs
username = "hongbaki"
#password = "put your password"
username_field_id = "username"  # Replace with the actual ID of the username field
password_field_id = "password"  # Replace with the actual ID of the password field

with open("password.txt", "r") as password_file:
    password = password_file.read().strip()

# Navigate to the login page
driver.get("https://auth.42.fr/auth/realms/students-42/protocol/openid-connect/auth?client_id=intra&redirect_uri=https%3A%2F%2Fprofile.intra.42.fr%2Fusers%2Fauth%2Fkeycloak_student%2Fcallback&response_type=code&state=ae365667127c864fe9576c8eb3f4285649467e597d498f6f")  # Replace with the actual login URL

time.sleep(1)

# Wait for the username field to be present
username_field = driver.find_element(By.ID, username_field_id)
#time.sleep(1)
username_field.send_keys(username)


# Enter the username

# Wait for 3 seconds
time.sleep(0.5)

# Press Tab to move to the next field
username_field.send_keys(Keys.TAB)
# Wait for another 3 seconds
#time.sleep(1)
# Find the password input field and enter the password
password_field = driver.find_element(By.ID, password_field_id)



time.sleep(0.5)

password_field.send_keys(password)
#time.sleep(1)
username_field.send_keys(Keys.ENTER)





# Wait for the slots to be loaded
#6.Wait for Slots to Load:
# Function to scroll down the page
# def scroll_page():
#     print("Scrolling through the page...")
#     last_height = driver.execute_script("return document.body.scrollHeight")
#     while True:
#         # Scroll down to the bottom of the page
#         driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
        
#         # Wait for new elements to load
#         time.sleep(5)

#         # Calculate new scroll height and compare with the last scroll height
#         new_height = driver.execute_script("return document.body.scrollHeight")
#         if new_height == last_height:
#             break
#         last_height = new_height
#     print("Finished scrolling.")
# # Navigate to the specified slots page
# # ... [Your existing navigation code] ...

# # Scroll through the page and check for open slots
# scroll_page()



# Navigate to the slots page
#5.Navigate to Slots Page:
#After logging in, the script navigates to the slots page where the available slots are listed.

#example of evaluation page
#https://projects.intra.42.fr/projects/cpp-module-09/slots?team_id=5374260

# Dynamically build the URL
project_names = ["libft",
                 "Born2beroot"
                 "ft_printf",
                 "get_next_line",
                 "so_long",
                 "fdf",
                 "fract-ol",
                 "push_swap",
                 "minitalk",
                 "pipex",
                 "philosophers",
                 "minishell",
                 "minirt",
                 "cub3d",
                 "netPractice",
                 "inception",
                 "webserv",
                 "ft_irc",
                 "cpp-module-00",
                 "cpp-module-01",
                 "cpp-module-02",
                 "cpp-module-03",
                 "cpp-module-04",
                 "cpp-module-05",
                 "cpp-module-06",
                 "cpp-module-07",
                 "cpp-module-08",
                 "cpp-module-09",
                 "ft_transcendence"]  

input_project_name = "cpp-module-09"

if input_project_name in project_names:
    base_url = "https://projects.intra.42.fr/projects"
    
    team_id = "5374260"  # Replace with the actual team ID
    full_url = f"{base_url}/{input_project_name}/slots?team_id={team_id}"
    print("Project exists. Full URL:", full_url)
else:
    print("Project does not exist.")
# Navigate to the specified slots page
driver.get(full_url)

time.sleep(2)





# Function to convert 12-hour format time to 24-hour format
def convert_to_24hr_format(time_str):
    try:
        return datetime.strptime(time_str, "%I:%M %p").strftime("%H:%M")
    except ValueError:
        print(f"Error converting time: {time_str}")
        return None



# Set the desired day for the slot
desired_day = datetime(2024, 1, 9).date()  # Year, Month, Day
# Set the desired start and end times
desired_start_time = datetime.strptime("08:15", "%H:%M").time()  # 24-hour format
# format_desired_start_time = desired_start_time.strftime("%H:%M")
# print("desired_start_time")
# print(desired_start_time)
# print("format_desired_start_time")
# print(format_desired_start_time)

desired_end_time = datetime.strptime("20:00", "%H:%M").time()  # 24-hour format
#format_desired_end_time = desired_end_time.strftime("%H:%M")
# print("desired_end_time")
# print(desired_end_time)
# print("format_desired_end_time")
# print(format_desired_end_time)

# This flag will indicate whether a slot has been successfully clicked
slot_clicked = False
max_retries = 100000
attempts = 0



while not slot_clicked and attempts < max_retries:
    try:
        attempts += 1  # Increment the attempt counter
        print(f"Attempt {attempts} of {max_retries}")




        # Wait until the slots are present on the page
        slots = WebDriverWait(driver, 3).until(
            EC.presence_of_all_elements_located((By.CLASS_NAME, "fc-title"))
        )
        # Check each slot to see if it is available and matches the desired time
        for slot in slots:
            print("00")
            
            slot_text = slot.text.strip()
            if "Available" in slot_text:
                print("10")
                print("11", desired_day)
                slot_day_elements = WebDriverWait(driver, 3).until(
                    EC.presence_of_all_elements_located((By.CSS_SELECTOR, ".fc-day-header.fc-widget-header.fc-tue"))
                )
                # Extract the date from the data-date attribute of the td element
                for slot_day_element in slot_day_elements:

                    slot_day_str = slot_day_element.get_attribute("data-date")
                    #slot_day = datetime.strptime(slot_day_str, "%Y-%m-%d").date()
                    print("20")
                    print("slot_day_str")
                    print(slot_day_str)
                    print("desired_day.strftime(%Y-%m-%d/)")
                    print(desired_day.strftime("%Y-%m-%d"))
                    if slot_day_str == desired_day.strftime("%Y-%m-%d"):
                    #if desired_day == slot_day:
                        #print("30")
                        # # Extract the end time from the data-start attribute
                        # slot_end_time_str = slot.find_element(By.CLASS_NAME, ".fc-time").get_attribute("data-end")
                        # slot_end_time = datetime.strptime(slot_end_time_str, "%H:%M").time()
                            
                        # Extract the start time from the data-start attribute
                        
                        # slot_start_times = WebDriverWait(driver, 0.5).until(
                        #     EC.presence_of_all_elements_located((By.CLASS_NAME, "fc-time"))
                        # )
                        slot_start_time_elements = driver.find_elements(By.CLASS_NAME, "fc-time")
                        print("31")

                        for slot_start_time_element in slot_start_time_elements:
                            print("40")
                            slot_start_time_str = slot_start_time_element.get_attribute("data-full")
                            
                            if slot_start_time_str:
                                # Extract only the time part, assuming format like "6:30 PM - 7:15 PM"
                                time_part = slot_start_time_str.split(" - ")[0]
                                # Convert to 24-hour format
                                slot_start_time_24hr = convert_to_24hr_format(time_part)
                                if slot_start_time_24hr:
                                    slot_start_time = datetime.strptime(slot_start_time_24hr, "%H:%M").time()
                                
                                    # slot_start_time_str = slot_start_time_element.get_attribute("data-start")
                                    print("41")
                                    # print("slot_start_time_str")
                                    # print(slot_start_time_str)

                                    # slot_start_time = datetime.strptime(slot_start_time_str, "%H:%M").time()
                                    # print("form_slot_start_time_str")
                                    # print(slot_start_time)

                                    # #slot_start_time = datetime.strptime(slot_start_time_str, "%H:%M").time()
                                    
                                    #slot_start_time_ = slot_start_time.strftime("%H:%M")
                                    
                                    # # Check if the slot's date and time match the desired criteria
                                    #if (desired_end_time.strftime("%H:%M") > slot_start_time.strftime("%H:%M")): 
                                    print("desired_start_time")
                                    print(desired_start_time)
                                    print("slot_start_time")
                                    print(slot_start_time)
                                    print("desired_end_time")
                                    print(desired_end_time)
                                
                                    if (desired_start_time <= slot_start_time <= desired_end_time):
                                        #and 
                                        #desired_start_time <= slot_start_time_str
                                        
                                        

                                        
                                        print(f"Found a matching slot: {slot_text}. Attempting to click...")
                                        
                                        slot.click()
                                        print("Clicked on the matching slot.")
                                        slot_clicked = True
                                        break
                        if slot_clicked:
                            break
                        try:
                        # Wait until the button is available and clickable
                            Cancel_button = WebDriverWait(driver, 10).until(
                                EC.element_to_be_clickable((By.XPATH, "//button[text()='Cancel']"))
                            )
                            # Click the button
                            Cancel_button.click()
                        except Exception as e:
                            print("Error:", e)
        
        if not slot_clicked:
            print("No matching slots found. Refreshing and retrying...")
            driver.refresh()        

    except TimeoutException:
        print("Timeout occurred while looking for slots. Refreshing and retrying...")
        driver.refresh()
    except NoSuchElementException:
        print("The slot element was not found. Refreshing and retrying...")
        driver.refresh()
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        break  # Exit the loop on unexpected errors

    # Add a delay before the next attempt to prevent overwhelming the server
    time.sleep(5)





   




# Check if the loop ended due to reaching the maximum number of retries
if attempts >= max_retries:
    print("Reached the maximum number of retries. Exiting.")

# Add code here to handle confirmation of booking if needed

# ... rest of your script ...
time.sleep(100)

# Close the WebDriver
#8.Close the WebDriver:
#This line closes the browser and ends the WebDriver's session. 
# It's important to include this to free up resources and not leave the browser running in the background.






#driver.quit()



