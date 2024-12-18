export default class FileApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async getFileByUserPath(userFilePath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/getFileContentByUserPath?userFilePath=${userFilePath}`);
    }

    async addFile(symbolFolderPath, pathToFile, description, isFrozen) {
      let requestBody = {};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
      requestBody["isFrozen"] = isFrozen;
    
      return await postJSON(`http://${this.#host}:${this.#port}/addNewFile`, requestBody);    
    }

    async appendVersion(pathToFile, description) {
      let requestBody = {};
      requestBody["pathToFile"] = pathToFile;
      requestBody["description"] = description;
    
      return await postJSON(`http://${this.#host}:${this.#port}/appendVersion`, requestBody);    
    }

    async deleteFile(symbolFilePath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/deleteFile?symbolPath=${symbolFilePath}`, "DELETE");
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

  async function postJSON(targetUrl, requestBody) {
    try {
      const response = await fetch(targetUrl, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(requestBody),
      });
  
      const result = await response.json();
      return result;
    } catch (error) {
      console.error("Error:", error);
    }
  }