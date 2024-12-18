export default class FolderApi {
    #host;
    #port;

    constructor(hostName, port) {
        this.#host = hostName;
        this.#port = port;
    }

    async getFolderContent(symbolFolderPath) {
        return await fetchJSON(`http://${this.#host}:${this.#port}/getFolderContent?symbolPath=${symbolFolderPath}`);
    }

    async getFolderByUserPath(userFolderPath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/getFolderContentByUserPath?userFolderPath=${userFolderPath}`);
    }

    async addFolder(symbolFolderPath, userFolderPath) {
      let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
      requestBody["symbolFolderPath"] = symbolFolderPath;
      requestBody["userFolderPath"] = userFolderPath;
    
      return await postJSON(`http://${this.#host}:${this.#port}/addNewFolder`, requestBody);    
    }

    async deleteFolder(symbolFolderPath) {
      return await fetchJSON(`http://${this.#host}:${this.#port}/deleteFolder?symbolPath=${symbolFolderPath}`, "DELETE");
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