use std::time::Duration;
use chrono::Datelike;
use crossterm::event::{self, Event, KeyCode};
use figlet_rs::FIGlet;
use ratatui::{
    prelude::*,
    widgets::*,
};

fn centered_rect(width: u16, height: u16, area: Rect) -> Rect {
    let width = width.min(area.width);
    let height = height.min(area.height);
    let x = area.x + (area.width.saturating_sub(width)) / 2;
    let y = area.y + (area.height.saturating_sub(height)) / 2;
    Rect::new(x, y, width, height)
}

fn weekday_label(weekday: chrono::Weekday) -> &'static str {
    match weekday {
        chrono::Weekday::Mon => "Monday",
        chrono::Weekday::Tue => "Tuesday",
        chrono::Weekday::Wed => "Wednesday",
        chrono::Weekday::Thu => "Thursday",
        chrono::Weekday::Fri => "Friday",
        chrono::Weekday::Sat => "Saturday",
        chrono::Weekday::Sun => "Sunday",
    }
}

fn ascii_text(font: &FIGlet, text: &str) -> String {
    font.convert(text).unwrap().to_string()
}

fn text_height(text: &str) -> u16 {
    text.lines().count() as u16
}

fn draw_info(frame: &mut Frame, font: &FIGlet) {
    let now = chrono::Local::now();
    let area = frame.area();

    let header_info = Paragraph::new("A-small-toy Terminal Date")
        .alignment(Alignment::Center)
        .style(Style::default().fg(Color::White));
    let header_area = Rect::new(0, 0, area.width, 1);
    frame.render_widget(header_info, header_area);

    let body_area = Rect::new(
        0,
        header_area.y + header_area.height,
        area.width,
        area.height.saturating_sub(2),
    );

    let date_str = now.format("%Y-%m-%d").to_string();
    let big_date = ascii_text(font, &date_str);
    let date_height = text_height(&big_date);

    let time_str = now.format("%H:%M:%S").to_string();
    let big_time = ascii_text(font, &time_str);
    let time_height = text_height(&big_time);

    let content_height = date_height + 1 + time_height;
    let available_gaps = body_area.height.saturating_sub(content_height);
    let gap_after_date = u16::from(available_gaps > 0);
    let gap_after_weekday = u16::from(available_gaps > 1);
    let stacked_height = content_height + gap_after_date + gap_after_weekday;
    let content_area = centered_rect(body_area.width, stacked_height, body_area);

    let mut current_y = content_area.y;

    let date_area = Rect::new(body_area.x, current_y, body_area.width, date_height);
    let date = Paragraph::new(big_date)
        .alignment(Alignment::Center)
        .style(Style::default().fg(Color::Cyan));
    frame.render_widget(date, date_area);
    current_y = current_y.saturating_add(date_height + gap_after_date);

    let weekday_area = Rect::new(body_area.x, current_y, body_area.width, 1);
    let weekday = Paragraph::new(weekday_label(now.weekday()))
        .alignment(Alignment::Center)
        .style(Style::default().fg(Color::Yellow));
    frame.render_widget(weekday, weekday_area);
    current_y = current_y.saturating_add(1 + gap_after_weekday);

    let paragraph = Paragraph::new(big_time)
        .alignment(Alignment::Center)
        .style(Style::default().fg(Color::Cyan));

    let time_area = Rect::new(body_area.x, current_y, body_area.width, time_height);
    frame.render_widget(paragraph, time_area);

    let help = Paragraph::new("Press 'q' or Esc to quit")
        .alignment(Alignment::Center)
        .style(Style::default().fg(Color::DarkGray));

    let help_area = Rect::new(0, area.height.saturating_sub(1), area.width, 1);
    frame.render_widget(help, help_area);
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let font = FIGlet::slant().expect("ERROR: Cannot load ascii font");
    let mut terminal = ratatui::init();

    loop {
        terminal.draw(|frame| {
            draw_info(frame, &font);
        })?;

        if event::poll(Duration::from_millis(200))? {
            if let Event::Key(key) = event::read()? {
                if key.code == KeyCode::Char('q') || key.code == KeyCode::Esc {
                    break;
                }
            }
        }
    }

    ratatui::restore();
    Ok(())
}
