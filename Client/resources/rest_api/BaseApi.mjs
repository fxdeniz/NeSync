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
  
async function postJSON(targetUrl, requestBody, methodType = 'POST') {
  try {
    const response = await fetch(targetUrl, {
      method: methodType,
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

class BaseApi {
  #host;
  #port;

  constructor(hostName, port) {
      this.host = hostName;
      this.port = port;
  }

  get host() {
    return this.#host;
  }

  set host(value) {
    this.#host = value;
  }

  get port() {
    return this.#port;
  }

  set port(value) {
    if (typeof value === 'number' && value >= 0 && value <= 65535)
      this.#port = value;
    else
      throw new TypeError(`Port number ${value} is not valid.`);
  }
}

  export {fetchJSON, postJSON, BaseApi};