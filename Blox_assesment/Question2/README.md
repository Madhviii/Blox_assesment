Question:

Banking works by transferring money from account A to account B. Most of the time
account A is in one bank while account B is another bank. Implement the code to transfer
money. Remember, payee's code runs on a different computer than that of the receiver's
code.


1. What are the issues in such a system?
    When transferring money from one bank (Bank A) to another (Bank B), several issues can arise:

    Network Failures: Since the code for the payee's account runs on a different machine (Bank A) than the receiver's account (Bank B), any network failure or latency can interrupt the transaction process.

    Atomicity: Transactions must be atomic, meaning that either all parts of the transaction complete successfully, or none do. If part of the transaction succeeds (e.g., money deducted from Bank A) and another fails (e.g., money not credited to Bank B), this leads to inconsistencies.

    Data Integrity: Ensuring that the amounts transferred are correct and reflect the true balances of the accounts at both banks is crucial. Any error in the amount could lead to significant financial discrepancies.

    Security: Sensitive financial data must be secured against unauthorized access and tampering during the transaction process.

    Concurrency: Multiple transactions can occur simultaneously, which may lead to race conditions where the same account is accessed and modified concurrently, causing potential data corruption.

2. What can we do to mitigate some of the issues ?
    To address these issues, we can implement several strategies:

    Use of a Two-Phase Commit Protocol (2PC): This ensures that all parties involved in the transaction agree on the completion of the transaction. If any party fails to confirm, the transaction is rolled back.

    Error Handling and Retries: Implement robust error handling to catch exceptions and retry operations as needed. This ensures that temporary network failures do not result in failed transactions.

    Transaction Logging: Maintain a log of all transactions to enable auditing and recovery in case of failures.

    Secure Communication: Use secure protocols (like HTTPS) to encrypt data in transit, ensuring that sensitive information is protected.

    Concurrency Control: Implement locking mechanisms or other concurrency control strategies to ensure that transactions are processed in a safe manner.


3. Write the fixing yourself to demonstrate the mitigations.
   Please refer the file Answer2.cpp for the code.

   Code Explaination:

    Mock API Calls: The callBankAPI function simulates a retry mechanism for network failures.

    Pre-Transfer Balance Check: Ensures the payer has sufficient funds before initiating the debit.
    
    Transaction Process: The transferFunds function first attempts to deduct funds from the payee’s account. If successful, it then attempts to credit the receiver’s account. If the second step fails, it rolls back the transaction by crediting the payee’s account back.

    Error Handling: The use of try-catch blocks allows the program to gracefully handle errors, outputting messages to indicate what went wrong.

    Simulated Security and Logging: In a real application, additional logging would be included to track successful transactions and errors for auditing and troubleshooting.
