export default class FolderApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async getFolderContent(symbolFolderPath) {
        return await fetchJSON(`http://$${this.#host}:${this.#port}/getFolderContent?symbolPath=${symbolFolderPath}`);
    }
}

async function fetchJSON(targetUrl, methodType = "GET") {
    try {
      const response = await fetch(targetUrl, {method: methodType});
      
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      
      const result = await response.json();
      
      return result;
  
    } catch (error) {
      console.error('There was a problem with the fetch operation:', error);
    }
  }