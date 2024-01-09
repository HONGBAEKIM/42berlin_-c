# 1.Imports
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
from bs4 import BeautifulSoup

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
username_field.send_keys(username)

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








# Navigate to the slots page
#5.Navigate to Slots Page:
#After logging in, the script navigates to the slots page where the available slots are listed.

#example of evaluation page
#https://projects.intra.42.fr/projects/cpp-module-09/slots?team_id=5374260

# Dynamically build the URL
base_url = "https://projects.intra.42.fr/projects"
project_name = "cpp-module-09"  # Replace with the actual project name

#everyone has different team_id
#forexample 
#hongbae has "5374260"
#Ramesh has  "5395823"

team_id = "5374260"  # Replace with the actual team ID 
full_url = f"{base_url}/{project_name}/slots?team_id={team_id}"

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
#desired_day = datetime(2024, 1, 10).date()  # Year, Month, Day
# Set the desired start and end times
                # print("desired_day")   
                # print(desired_day)   
desired_start_time = datetime.strptime("20:00", "%H:%M").time()  # 24-hour format
desired_end_time = datetime.strptime("23:30", "%H:%M").time()  # 24-hour format


# This flag will indicate whether a slot has been successfully clicked
slot_clicked = False
max_retries = 100000
attempts = 0


# Function to check if the slot time is within the desired range
def is_time_within_range(time_str, start_time, end_time):
    slot_time = datetime.strptime(time_str, "%I:%M %p").time()
    return start_time <= slot_time <= end_time


while not slot_clicked and attempts < max_retries:
    try:
        attempts += 1
        print(f"Attempt {attempts} of {max_retries}")
        time.sleep(1)

        todays_date_str = datetime.today().strftime("%Y-%m-%d")
        print("00 : yoyo ")
        try:
            print("10 : gogo ")
            
            #initialize today_colume with 'fc-state-highlight' from html
            today_column = driver.find_element(By.CSS_SELECTOR, f"td.fc-day.fc-today.fc-state-highlight[data-date='{todays_date_str}']")
            print("11 : Found today_column")
       
            available_slots_today = []
            print("12 : Before finding slots")

            
            #.//: This means the search should start from the current node (today_column). 
            #The . denotes the current node, 
            #and // means to search at any depth below the current node.

            #following-sibling::td: This part of the XPath selects all <td> elements 
            #that are following siblings of the current node (today_column). 
            #In an HTML table structure, sibling <td> elements are typically cells in the same row 
            #as the current cell.

            #//div[contains(@class, 'fc-time')]: 
            #This further filters down to <div> elements under those <td> elements. 
            #The contains(@class, 'fc-time') function selects <div> elements 
            #that have a class attribute containing the text fc-time. 
            #The contains function is useful because class attributes 
            #can have multiple space-separated values, 
            #and this function will match as long as one of those values is fc-time.
            slots = today_column.find_elements(By.XPATH, ".//following-sibling::td//div[contains(@class, 'fc-time')]")
            print("13 : After finding slots, count:", len(slots))

            for slot in slots:
                print("20 : Inside slot loop")
                time_str = slot.get_attribute("data-full").split(" - ")[0]
                print("21 : now we have time_str")
                
                if is_time_within_range(convert_to_24hr_format(time_str), desired_start_time, desired_end_time):
                    print("30 : check time range")
                    available_slots_today.append(slot)

            if not available_slots_today:
                print("No slots available within the desired time range.")
                driver.refresh()
                time.sleep(5)
                continue

            for slot in available_slots_today:
                print("40")
                WebDriverWait(driver, 10).until(EC.element_to_be_clickable(slot))
                print("41")
                slot.click()
                print("Clicked on an available slot.")
                slot_clicked = True
                break

        except NoSuchElementException:
            print("Today's column is not found or not highlighted.")
            driver.refresh()
            time.sleep(5)

    except TimeoutException:
        print("Timeout occurred while looking for slots. Refreshing and retrying...")
        driver.refresh()
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        break

    time.sleep(3)

if attempts >= max_retries:
    print("Reached the maximum number of retries. Exiting.")





time.sleep(20)

# while not slot_clicked and attempts < max_retries:
#     try:
#         attempts += 1  # Increment the attempt counter
#         print(f"Attempt {attempts} of {max_retries}")

#         # Wait until the slots are present on the page
#         slots = WebDriverWait(driver, 3).until(
#             EC.presence_of_all_elements_located((By.CLASS_NAME, "fc-title"))
#         )
#         # Check each slot to see if it is available and matches the desired time
#         for slot in slots:
#             print("00")
            
#             slot_text = slot.text.strip()
#             if "Available" in slot_text:
#                 print("10")

                  
#                 slot_start_time_elements = driver.find_elements(By.CLASS_NAME, "fc-time")                  
#                 print("12")

#                 for slot_start_time_element in slot_start_time_elements:
#                     print("20")
#                     slot_start_time_str = slot_start_time_element.get_attribute("data-full")
                    
#                     if slot_start_time_str:
#                         # Extract only the time part, assuming format like "6:30 PM - 7:15 PM"
#                         time_part = slot_start_time_str.split(" - ")[0]
#                         # Convert to 24-hour format
#                         slot_start_time_24hr = convert_to_24hr_format(time_part)
#                         if slot_start_time_24hr:
#                             print("30")
                            
#                             slot_start_time = datetime.strptime(slot_start_time_24hr, "%H:%M").time()

#                             print("31")
                            
#                             print("desired_start_time")
#                             print(desired_start_time)
#                             print("slot_start_time")
#                             print(slot_start_time)
#                             print("desired_end_time")
#                             print(desired_end_time)
                        
#                             if (desired_start_time <= slot_start_time <= desired_end_time):
#                                 print("40")
#                                 # Assuming you have already loaded the HTML page and have it in a variable called 'html_content'
#                                 # You can extract the desired date as follows:


#                                 #i want to implement with data.
#                                 #but I dont know how to implement
#                                 #idea is desired_date should be same 
#                                 html_content = driver.page_source
#                                 soup = BeautifulSoup(html_content, 'html.parser')
#                                 if soup:
#                                     print("soup is available")
#                                 else:
#                                     print("soup has not found")
#                                 # Find a <td> element with a class that contains 'fc-state-highlight'
#                                 highlighted_td = soup.select_one('td[class*="fc-state-highlight"]')

#                                 # Check if the element exists and has the 'data-date' attribute
#                                 if highlighted_td:
#                                     desired_day = highlighted_td.get('data-date')
#                                     print(f"The desired day is: {desired_day}")
#                                 else:
#                                     print("Desired day not found in the HTML.")
                                
                                
#                                 # if (desired_day == )

#                                 #     print(f"Found a matching slot: {slot_text}. Attempting to click...")
                                    
#                                 #     slot.click()
#                                 #     print("Clicked on the matching slot.")
#                                 #     slot_clicked = True
#                                 #     break
#                                 # else:
#                                 #     print("desired_day == ????????")
#                 if slot_clicked:
#                     break
#         if not slot_clicked:
#             print("No matching slots found. Refreshing and retrying...")
#             driver.refresh()        

#     except TimeoutException:
#         print("Timeout occurred while looking for slots. Refreshing and retrying...")
#         driver.refresh()
#     except NoSuchElementException:
#         print("The slot element was not found. Refreshing and retrying...")
#         driver.refresh()
#     except Exception as e:
#         print(f"An unexpected error occurred: {e}")
#         break  # Exit the loop on unexpected errors

#     # Add a delay before the next attempt to prevent overwhelming the server
#     time.sleep(5)

# # Check if the loop ended due to reaching the maximum number of retries
# if attempts >= max_retries:
#     print("Reached the maximum number of retries. Exiting.")

# # Add code here to handle confirmation of booking if needed

# # ... rest of your script ...

# Close the WebDriver
#8.Close the WebDriver:
#This line closes the browser and ends the WebDriver's session. 
# It's important to include this to free up resources and not leave the browser running in the background.
#driver.quit()



