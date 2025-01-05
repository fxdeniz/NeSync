import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const symbolPath = await window.appState.get("currentFile");

    const fileApi = new FileApi("localhost", 1234);
    const info = await fileApi.get(symbolPath)

    const divFileName = document.getElementById("div-file-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    divFileName.textContent = info.fileName;
    inputCurrentPath.value = info.symbolFilePath;

    const ulVersions = document.getElementById("ul-versions");
    info.versionList.forEach(item => {
        ulVersions.appendChild(createListItem(item.versionNumber, item.description, item.size, item.lastModifiedTimestamp));
    });
});

function createListItem(versionNumber, description, fileSize, timestamp) {
    const button = document.createElement("button");
    button.type = "button";
    button.className = "list-group-item list-group-item-action text-center";

    const span = document.createElement("span");
    span.className = "badge rounded-pill text-bg-primary";
    span.textContent = versionNumber;

    button.appendChild(span);
    button.addEventListener("click", () => {
        const pDescription = document.getElementById("p-description");
        const hFileSize = document.getElementById("h-file-size");
        const divTimePassed = document.getElementById("div-time-passed");
    
        pDescription.innerHTML = description;
        hFileSize.innerText = formatFileSize(fileSize);
        divTimePassed.innerText = timePassed(timestamp);
    });

    return button;
}

function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    
    const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
    const index = Math.floor(Math.log(bytes) / Math.log(1024));
    const formattedSize = (bytes / Math.pow(1024, index)).toFixed(2);
    
    return `${formattedSize} ${sizes[index]}`;
}

function timePassed(date) {
    const now = new Date();
    const past = new Date(date);

    if (isNaN(past)) return "Invalid date";

    const diffInSeconds = Math.floor((now - past) / 1000);
    const seconds = diffInSeconds % 60;
    const minutes = Math.floor(diffInSeconds / 60) % 60;
    const hours = Math.floor(diffInSeconds / 3600) % 24;
    const days = Math.floor(diffInSeconds / (3600 * 24));
    const weeks = Math.floor(days / 7);
    const months = Math.floor(days / 30); // Approximate
    const years = Math.floor(days / 365); // Approximate

    // Format the original date
    const monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
    const originalDate = `${past.getFullYear()} ${monthNames[past.getMonth()]} ${String(past.getDate()).padStart(2, '0')} ${String(past.getHours()).padStart(2, '0')}:${String(past.getMinutes()).padStart(2, '0')}`;

    // If it's today, return in hours and minutes
    if (days === 0) {
        if (hours > 0) return `${hours} hours ${minutes} minutes ago - ${originalDate}`;
        if (minutes > 0) return `${minutes} minutes ago - ${originalDate}`;
        return `${seconds} seconds ago - ${originalDate}`;
    }

    // Otherwise, format as years, months, weeks, and days
    let result = "";
    if (years > 0) result += `${years} years `;
    if (months % 12 > 0) result += `${months % 12} months `;
    if (weeks % 4 > 0) result += `${weeks % 4} weeks `;
    if (days % 7 > 0) result += `${days % 7} days `;

    return `${result.trim()} ago - ${originalDate}`;
}