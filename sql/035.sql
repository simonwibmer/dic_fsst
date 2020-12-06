select round(sum(salary)),round(max(salary)),round(min(salary)),round(avg(salary)),job_id from employees where department_id='90'  group by job_id;
