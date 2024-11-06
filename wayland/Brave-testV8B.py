import os
import re
import time
import subprocess
from datetime import datetime
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
current_date=datetime.now().strftime('%Y%m%d-%H%M%S')
url_with_query=f"https://www.google.com/search?q={search_query.replace(' ', '+')}+{datetime.now().strftime('%d/%m/%Y').replace('/', '%2F')}"

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
    filepath=os.path.join(screenshot_dir, filename.replace('.', '-').rsplit('-', 1)[0] + ".png")
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
    """Extract valid URLs from text content, replacing non-standard characters."""
    urls=[]
    for line in content.split('\n'):
        # Replace `›` with `/` for non-standard URLs
        line=line.replace('›', '/')
        # Match URLs starting with http or https
        matches=re.findall(r'https?://[^\s]+', line)
        
        for match in matches:
            try:
                response=requests.get(match)
                if response.status_code == 200:
                    urls.append(match)
            except requests.RequestException as e:
                print(f"Failed to fetch URL {match}: {e}")
                
    return list(set(urls))[:6]  # Limit to the first 2 unique URLs

# Clear clipboard
copy_to_clipboard("")

# Visit the search page and ensure the first page is copied correctly
subprocess.run(['xdg-open', url_with_query])
time.sleep(3)

# Confirm text is selected visually, then retry copy if necessary
if confirm_text_selected():
    clipboard_content=ensure_copy_success()
    if clipboard_content:
        filename=os.path.join(search_dir, f"{formatted_query}-{current_date}.txt")
        with open(filename, 'w') as f:
            f.write('\n'.join(clipboard_content.split('\n')[:-3]))
        print(f"Saved to file: {filename}")
    else:
        print("No content found in the clipboard after multiple attempts.")
else:
    print("Text not selected. Exiting.")

def visit_and_save_urls(urls):
    """Visit each valid URL and save the page content."""
    source_links=[]
    for i, url in enumerate(urls):
        print(f"Opening URL {i+1}/{len(urls)}: {url}")
        subprocess.Popen([brave_browser_path, '--new-tab', url])
        time.sleep(3)

        if confirm_text_selected():
            clipboard_content=ensure_copy_success()
            if clipboard_content:
                source_links.append(f"[Source {i+1}]({url})")
                url_filename=os.path.join(search_dir, f"{url.split('//')[1].split('/')[0]}-{current_date}.txt")
                with open(url_filename, 'w') as f:
                    f.write(clipboard_content)
                print(f"Page content saved to file: {url_filename}")
            else:
                print(f"No content found in the clipboard for URL: {url}")
        else:
            print(f"Text not selected on {url}. Skipping.")

    return source_links

# Extract valid URLs from the initial file content
with open(filename, 'r') as f:
    file_content=f.read()

valid_urls=get_valid_urls(file_content)
source_links=visit_and_save_urls(valid_urls)

def summarize_content(content):
    """Use Ollama to summarize the content in English."""
    if not content.strip():
        print("No content to summarize.")
        return None

    ollama_model=Ollama(model="crewai-llama3.2")
    summary=ollama_model.invoke(input=f"Summarize the following text into a concise article:\n\n{content}")

    return summary

def generate_summary(search_dir, source_links):
    """Generate a summary for the saved articles and include sources."""
    summary_file=os.path.join(search_dir, f"summary-{current_date}.txt")
    combined_content=""
    
    for file in os.listdir(search_dir):
        if file.endswith('.txt') and 'results' not in file:
            with open(os.path.join(search_dir, file), 'r') as f:
                combined_content += f.read() + "\n"
    
    if combined_content:
        print("Generating summary...")
        summary=summarize_content(combined_content)
        summary=re.sub(r'\s+', ' ', summary).strip()
        
        with open(summary_file, 'w') as f:
            f.write(summary + "\n\nSources:\n" + "\n".join(source_links))
        
        print(f"Summary saved to: {summary_file}")
        return summary_file
    else:
        print("No content to summarize.")
        return None

# Generate and save summary of the articles
summary_file=generate_summary(search_dir, source_links)

# Open the summary file if it was created successfully
if summary_file:
    subprocess.run(['xdg-open', summary_file])

