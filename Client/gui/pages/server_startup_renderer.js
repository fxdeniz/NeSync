import MonitorApi from "../rest_api/MonitorApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {

    const isPacked = await window.appState.isPacked();

    if(isPacked) {
        window.serverProcess.run();
        
        setTimeout(async () => {
            const port = await window.appState.get("serverPort");
            const pid = await window.appState.get("serverPid");
            const monitorApi = new MonitorApi("localhost", port);
            console.log(`requesting pid ${pid} from port ${port}`);
            const heartbeat = await monitorApi.heartbeat(); // TODO: handle cases when server is not running.
    
            if(heartbeat.pid === pid)
                window.router.routeToFileExplorer();
        }, 15000);    
    }
    else
        alert("in debug");
    
});