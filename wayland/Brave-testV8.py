import os
import re
from datetime import datetime
import time
import subprocess
import requests
from dotenv import load_dotenv
from langchain_community.llms import Ollama
import pytesseract
from autocorrect import Speller

# Load environment variables from .env file
load_dotenv()
OLLAMA_API_KEY=os.getenv('OLLAMA_API_KEY')
OPENAI_API_KEY=os.getenv('OPENAI_API_KEY')

# Initialize the spell checker
spell=Speller(lang='en')

# Set the path for Tesseract executable if not in your PATH
pytesseract.pytesseract.tesseract_cmd='/usr/bin/tesseract'

# Define the path for Brave Browser
brave_browser_path="/usr/bin/brave-browser"

# Define the search query and URL
search_query="AI Robotics News"
formatted_query=search_query.replace(" ", "-")
current_date=datetime.now().strftime('%d/%m/%Y')
url_with_query=f"https://www.google.com/search?q={search_query.replace(' ', '+')}+{current_date.replace('/', '%2F')}"

# Create directories for saving files
base_dir="searches"
search_dir=os.path.join(base_dir, formatted_query)
os.makedirs(search_dir, exist_ok=True)

def get_clipboard_content():
    """Fetch clipboard content using xclip."""
    result=subprocess.run(['xclip', '-selection', 'clipboard', '-o'], stdout=subprocess.PIPE, text=True)
    return result.stdout.strip()

def copy_to_clipboard(content):
    """Copy content to clipboard using xclip."""
    subprocess.run(['xclip', '-selection', 'clipboard'], input=content.encode('utf-8'))

def simulate_copy():
    """Simulate Ctrl+A and Ctrl+C using xdotool to copy content."""
    subprocess.run(['xdotool', 'key', 'ctrl+a'])
    time.sleep(0.5)
    subprocess.run(['xdotool', 'key', 'ctrl+c'])
    time.sleep(0.5)

def take_screenshot(filename="screenshot.png"):
    """Take a screenshot using gnome-screenshot."""
    screenshot_dir=os.path.join(search_dir, "screenshots")
    os.makedirs(screenshot_dir, exist_ok=True)
    filepath=os.path.join(screenshot_dir, filename)
    subprocess.run(['gnome-screenshot', '-f', filepath])
    print(f"Screenshot saved to {filepath}")
    return filepath

def confirm_text_selected():
    """Confirm text is selected by taking a screenshot and performing OCR to verify non-empty text."""
    screenshot_path=take_screenshot("selection_verification.png")
    text_from_screenshot=pytesseract.image_to_string(screenshot_path)
    return bool(text_from_screenshot.strip())

def ensure_copy_success():
    """Ensure content is copied to clipboard by verifying non-empty clipboard or retrying if necessary."""
    attempt=1
    max_attempts=3
    while attempt <= max_attempts:
        print(f"Attempt {attempt}: Trying to copy content from the page...")
        simulate_copy()
        clipboard_content=get_clipboard_content()
        
        if clipboard_content:
            print("Text copied successfully.")
            return clipboard_content
        else:
            print("Copy attempt failed. Retrying...")
            take_screenshot(f"attempt_{attempt}_failed.png")
            attempt += 1
            time.sleep(2)
            
    print("Failed to copy text after several attempts.")
    return None

def get_valid_urls(content):
    """Extract valid URLs from text content."""
    urls=[]
    for line in content.split('\n'):
        if line.startswith('http'):
            try:
                response=requests.get(line)
                if response.status_code == 200:
                    urls.append(line)
            except requests.RequestException as e:
                print(f"Failed to fetch URL {line}: {e}")
    return list(set(urls))[:10]

# Clear clipboard
copy_to_clipboard("")

# Visit the search page and ensure the first page is copied correctly
subprocess.run(['xdg-open', url_with_query])
time.sleep(3)

# Confirm text is selected visually, then retry copy if necessary
if confirm_text_selected():
    clipboard_content=ensure_copy_success()
    if clipboard_content:
        filename=os.path.join(search_dir, f"results-{datetime.now().strftime('%Y%m%d-%H%M%S')}.txt")
        with open(filename, 'w') as f:
            f.write('\n'.join(clipboard_content.split('\n')[:-3]))
        print(f"Saved to file: {filename}")
    else:
        print("No content found in the clipboard after multiple attempts.")
else:
    print("Text not selected. Exiting.")

def visit_and_save_urls(urls):
    """Visit each valid URL and save the page content."""
    for i, url in enumerate(urls):
        print(f"Opening URL {i+1}/{len(urls)}: {url}")
        subprocess.Popen([brave_browser_path, '--new-tab', url])
        time.sleep(3)

        # Confirm text is selected and retry copy if necessary
        if confirm_text_selected():
            clipboard_content=ensure_copy_success()
            if clipboard_content:
                url_filename=os.path.join(search_dir, f"{url.split('//')[1].split('/')[0]}-{datetime.now().strftime('%Y%m%d')}.txt")
                with open(url_filename, 'w') as f:
                    f.write(clipboard_content)
                print(f"Page content saved to file: {url_filename}")
            else:
                print(f"No content found in the clipboard for URL: {url}")
        else:
            print(f"Text not selected on {url}. Skipping.")

# Extract valid URLs from the initial file content
with open(filename, 'r') as f:
    file_content=f.read()

valid_urls=get_valid_urls(file_content)
visit_and_save_urls(valid_urls)

def summarize_content(file_path):
    """Use Ollama to summarize the content of the given file."""
    with open(file_path, 'r') as f:
        content=f.read()

    if not content.strip():
        print("No content to summarize.")
        return None

    # Initialize Ollama model (no api_key needed)
    ollama_model=Ollama(model="crewai-llava-llama3:latest")

    # Call the model to summarize the content
    summary=ollama_model.invoke(input=f"Summarize the following text into a concise article:\n\n{content}")

    return summary

# After saving the results to a file
summary=summarize_content(filename)

if summary:
    summary_filename=os.path.join(search_dir, f"summary-{datetime.now().strftime('%Y%m%d-%H%M%S')}.txt")
    with open(summary_filename, 'w') as f:
        f.write(summary)
    print(f"Summary saved to file: {summary_filename}")
    
    # Open the summary file in Brave Browser
    subprocess.run([brave_browser_path, summary_filename])
else:
    print("Failed to generate a summary.")

def generate_summary(search_dir):
    """Generate a summary for the saved articles."""
    summary_file=os.path.join(search_dir, f"summary-{datetime.now().strftime('%Y%m%d-%H%M%S')}.txt")
    combined_content=""
    
    for file in os.listdir(search_dir):
        if file.endswith('.txt') and 'results' not in file:
            with open(os.path.join(search_dir, file), 'r') as f:
                combined_content += f.read() + "\n"
    
    if combined_content:
        print("Generating summary...")
        summary=summarize_content(combined_content)
        
        # Clean up the summary (remove malformed or unnecessary parts)
        summary=re.sub(r'\s+', ' ', summary).strip()  # Replace multiple spaces with a single space
        
        with open(summary_file, 'w') as f:
            f.write(summary)
        
        print(f"Summary saved to: {summary_file}")
        return summary_file
    else:
        print("No content to summarize.")
        return None

# Generate and save summary of the articles
summary_file=generate_summary(search_dir)

# Open the summary file if it was created successfully
if summary_file:
    subprocess.run(['xdg-open', summary_file])

