import re
import os
import asyncio
from urllib.parse import urldefrag
from readability.readability import Document
from bs4 import BeautifulSoup
from crawl4ai import (
    AsyncWebCrawler, BrowserConfig, CrawlerRunConfig, CacheMode,
    MemoryAdaptiveDispatcher
)


async def crawl_recursive_batch(start_urls, max_depth=3, max_concurrent=10, output_path="raw_dataset.txt", log_fn=print):
    browser_config = BrowserConfig(headless=True, verbose=False)
    run_config = CrawlerRunConfig(
        cache_mode=CacheMode.BYPASS,
        stream=False
    )
    dispatcher = MemoryAdaptiveDispatcher(
        memory_threshold_percent=70.0,      # Don't exceed 70% memory usage
        check_interval=1.0,                 # Check memory every second
        max_session_permit=max_concurrent   # Max parallel browser sessions
    )

    visited = set()
    all_text = []

    def normalize_url(url):
        return urldefrag(url)[0]

    current_urls = set([normalize_url(u) for u in start_urls])

    async with AsyncWebCrawler(config=browser_config) as crawler:
        for depth in range(max_depth):
            log_fn(f"\n=== Crawling Depth {depth+1} ===")
            urls_to_crawl = [normalize_url(url) for url in current_urls if normalize_url(url) not in visited]

            if not urls_to_crawl:
                break

            results = await crawler.arun_many(
                urls=urls_to_crawl,
                config=run_config,
                dispatcher=dispatcher
            )

            next_level_urls = set()

            for result in results:
                norm_url = normalize_url(result.url)
                visited.add(norm_url)

                # new bolck
                if result.success and result.html:
                    try:
                        doc = Document(result.html)
                        summary_html = doc.summary()
                        soup = BeautifulSoup(summary_html, "html.parser")
                        text = soup.get_text(separator="\n")

                        cleaned = "\n".join(line.strip() for line in text.splitlines() if line.strip())
                        all_text.append(cleaned)

                        log_fn(f"[OK] {result.url} | Readability text: {len(cleaned)} chars")
                    except Exception as e:
                        log_fn(f"[ERROR parsing] {result.url}: {e}")

                for link in result.links.get("internal", []):
                    next_url = normalize_url(link["href"])
                    if next_url not in visited:
                        next_level_urls.add(next_url)
                else:
                    log_fn(f"[ERROR] {result.url}: {result.error_message}")
                    
            current_urls = next_level_urls

    # Save final dataset
    output_path = output_path
    with open(output_path, "w", encoding="utf-8") as f:
        f.write("\n".join(all_text))
    log_fn(f"\nDataset saved with {len(all_text)} pages.")


def clean_text_for_llm(raw_path="raw_dataset.txt", final_path="cleaned_dataset.txt"):
    with open(raw_path, "r", encoding="utf-8") as f:
        raw_text = f.read()

    # Split by double line breaks = page or topic boundaries
    chunks = raw_text.split("\n\n")

    cleaned_chunks = []
    for chunk in chunks:
        # Remove markdown links [text](url)
        chunk = re.sub(r"\[([^\]]+)\]\((https?://[^\)]+)\)", r"\1", chunk)

        # Remove raw URLs
        chunk = re.sub(r"https?://\S+", "", chunk)

        # Remove excess whitespace
        chunk = re.sub(r"[ \t]+", " ", chunk)
        chunk = re.sub(r"\n\s*", " ", chunk)
        chunk = re.sub(r"(?<=\w)\n(?=\w)", " ", chunk)

        # Remove space before punctuation like . , ! ? ; :
        chunk = re.sub(r"\s+([.,!?;:])", r"\1", chunk)

        # Strip leading/trailing
        chunk = chunk.strip()

        if chunk:
            cleaned_chunks.append(chunk)

    final_text = "\n".join(cleaned_chunks)

    mode = 'a' if os.path.exists(final_path) else 'w'
    with open(final_path, mode, encoding='utf-8') as f:
        if mode == 'a':
            f.write('\n')  # Ensure it starts on a new line
        f.write(final_text)

    print(f"Cleaned dataset saved to '{final_path}'")


def extract_urls_from_file(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Split by *comma, space, or newline
    raw_links = re.split(r'[\s]+', content)

    # Remove empty strings and strip whitespace
    links = [link.strip() for link in raw_links if link.strip()]

    return links


if __name__ == "__main__":
    links = extract_urls_from_file("links.txt")
    print(f"Starting crawler on the following links: \n{links}")
    asyncio.run(crawl_recursive_batch(links, max_depth=1, max_concurrent=10))
    clean_text_for_llm()