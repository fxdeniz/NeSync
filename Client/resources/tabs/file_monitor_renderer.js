document.addEventListener("DOMContentLoaded", async (event) => {
    console.log(await fetchJSON('http://localhost:1234/startMonitoring'));
});


async function fetchJSON(targetUrl) {
    try {
      const response = await fetch(targetUrl);
      
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      
      const result = await response.json();
      
      return result;
  
    } catch (error) {
      console.error('There was a problem with the fetch operation:', error);
    }
  }